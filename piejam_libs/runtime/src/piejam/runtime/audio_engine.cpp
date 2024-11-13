// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/audio_engine.h>

#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/components/make_fx.h>
#include <piejam/runtime/components/mixer_channel.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/components/solo_switch.h>
#include <piejam/runtime/dynamic_key_shared_object_map.h>
#include <piejam/runtime/external_audio.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/runtime/processors/midi_assignment_processor.h>
#include <piejam/runtime/processors/midi_input_processor.h>
#include <piejam/runtime/processors/midi_learn_processor.h>
#include <piejam/runtime/processors/midi_to_parameter_processor.h>
#include <piejam/runtime/processors/stream_processor_factory.h>
#include <piejam/runtime/solo_group.h>
#include <piejam/runtime/state.h>

#include <piejam/algorithm/for_each_adjacent.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/engine/clip_processor.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_algorithms.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/input_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/output_processor.h>
#include <piejam/audio/engine/process.h>
#include <piejam/audio/engine/stream_processor.h>
#include <piejam/audio/engine/value_io_processor.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/midi/event.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/worker.h>

#include <fmt/format.h>

#include <boost/hof/match.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <algorithm>
#include <fstream>
#include <optional>
#include <ranges>

namespace piejam::runtime
{

namespace
{

enum class engine_processors
{
    midi_input,
    midi_learn,
    midi_assign
};

struct mixer_input_key
{
    mixer::channel_id channel_id;
    mixer::io_address_t route;

    constexpr bool operator==(mixer_input_key const&) const noexcept = default;
};

struct mixer_output_key
{
    mixer::channel_id channel_id;

    constexpr bool operator==(mixer_output_key const&) const noexcept = default;
};

struct mixer_aux_send_key
{
    mixer::channel_id channel_id;
    mixer::io_address_t route;

    constexpr bool
    operator==(mixer_aux_send_key const&) const noexcept = default;
};

struct solo_group_key
{
    constexpr bool operator==(solo_group_key const&) const noexcept = default;
};

using processor_map = dynamic_key_shared_object_map<audio::engine::processor>;
using component_map = dynamic_key_shared_object_map<audio::engine::component>;

using processor_ptr = std::unique_ptr<audio::engine::processor>;
using component_ptr = std::unique_ptr<audio::engine::component>;

template <class T>
using value_io_processor_ptr =
        std::unique_ptr<audio::engine::value_io_processor<T>>;

using get_device_processor_f =
        std::function<audio::engine::processor&(std::size_t)>;

void
make_mixer_components(
        component_map& comps,
        component_map& prev_comps,
        audio::sample_rate const sample_rate,
        strings_t const& strings,
        mixer::channels_t const& channels,
        parameter_processor_factory& param_procs,
        processors::stream_processor_factory& stream_procs)
{
    for (auto const& [mixer_channel_id, mixer_channel] : channels)
    {
        mixer_input_key const in_key{
                .channel_id = mixer_channel_id,
                .route = mixer_channel.in};
        if (auto comp = prev_comps.find(in_key))
        {
            comps.insert(in_key, std::move(comp));
        }
        else
        {
            comps.insert(
                    in_key,
                    components::make_mixer_channel_input(mixer_channel));
        }

        mixer_output_key const out_key{.channel_id = mixer_channel_id};
        if (auto comp = prev_comps.find(out_key))
        {
            comps.insert(out_key, std::move(comp));
        }
        else
        {
            comps.insert(
                    out_key,
                    components::make_mixer_channel_output(
                            mixer_channel,
                            *strings[mixer_channel.name],
                            param_procs,
                            stream_procs,
                            sample_rate));
        }

        for (auto const& [aux, aux_send] : *mixer_channel.aux_sends)
        {
            if (!aux_send.enabled)
            {
                continue;
            }

            mixer_aux_send_key const aux_send_key{
                    .channel_id = mixer_channel_id,
                    .route = aux};

            if (auto comp = prev_comps.find(aux_send_key))
            {
                comps.insert(aux_send_key, std::move(comp));
            }
            else
            {
                comps.insert(
                        aux_send_key,
                        components::make_mixer_channel_aux_send(
                                *strings[mixer_channel.name],
                                aux_send.volume,
                                param_procs));
            }
        }
    }
}

void
make_solo_group_components(
        component_map& comps,
        solo_groups_t const& solo_groups,
        parameter_processor_factory& param_procs)
{
    comps.insert(
            solo_group_key{},
            components::make_solo_switch(solo_groups, param_procs));
}

auto
make_fx_chain_components(
        component_map& comps,
        component_map& prev_comps,
        fx::modules_t const& fx_modules,
        parameters_map const& params,
        parameter_processor_factory& param_procs,
        processors::stream_processor_factory& stream_procs,
        fx::simple_ladspa_processor_factory const& ladspa_fx_proc_factory,
        audio::sample_rate const sample_rate)
{
    auto get_fx_param_name =
            [&params](parameter_id param_id) -> std::string_view {
        return std::visit(
                [&](auto typed_param_id) -> std::string_view {
                    return *params[typed_param_id].param.name;
                },
                param_id);
    };

    for (auto const& [fx_mod_id, fx_mod] : fx_modules)
    {
        if (fx_mod.bypassed)
        {
            continue;
        }

        if (auto fx_comp = prev_comps.find(fx_mod_id))
        {
            comps.insert(fx_mod_id, std::move(fx_comp));
        }
        else
        {
            auto comp = components::make_fx(
                    fx_mod,
                    get_fx_param_name,
                    ladspa_fx_proc_factory,
                    param_procs,
                    stream_procs,
                    sample_rate);
            if (comp)
            {
                comps.insert(fx_mod_id, std::move(comp));
            }
        }
    }
}

auto
make_midi_processors(
        std::unique_ptr<midi::input_event_handler> midi_in,
        bool const midi_learning,
        processor_map& procs)
{
    if (midi_in)
    {
        procs.insert(
                engine_processors::midi_input,
                processors::make_midi_input_processor(std::move(midi_in)));

        if (midi_learning)
        {
            procs.insert(
                    engine_processors::midi_learn,
                    processors::make_midi_learn_processor());
        }
    }
}

auto
make_midi_assignment_processors(
        midi_assignments_map const& assignments,
        parameters_map const& params,
        processor_map& procs,
        processor_map& prev_procs)
{
    if (assignments.empty())
    {
        return;
    }

    procs.insert(
            engine_processors::midi_assign,
            processors::make_midi_assignment_processor(assignments));

    for (auto const& [param_id, assignment] : assignments)
    {
        std::visit(
                [&]<class ParamId>(ParamId const typed_param_id) {
                    auto const& param = params[typed_param_id].param;

                    std::tuple const proc_id{typed_param_id, assignment};
                    if (auto proc = prev_procs.find(proc_id))
                    {
                        procs.insert(proc_id, std::move(proc));
                    }
                    else if constexpr (is_midi_assignable_v<ParamId>)
                    {
                        procs.insert(
                                proc_id,
                                processors::make_midi_cc_to_parameter_processor(
                                        param));
                    }
                },
                param_id);
    }
}

auto
connect_fx_chain(
        audio::engine::graph& g,
        std::vector<audio::engine::component*> const& fx_chain_comps)
{
    for (auto comp : fx_chain_comps)
    {
        comp->connect(g);
    }

    algorithm::for_each_adjacent(
            fx_chain_comps,
            [&](auto const l_comp, auto const r_comp) {
                connect(g, *l_comp, *r_comp);
            });
}

auto
connect_mixer_channel_with_fx_chain(
        audio::engine::graph& g,
        component_map const& comps,
        audio::engine::component& mixer_channel_in,
        fx::chain_t const& fx_chain,
        audio::engine::component& mixer_channel_out)
{
    auto fx_chain_comps = algorithm::transform_to_vector(
            fx_chain,
            [&comps](fx::module_id fx_mod_id) {
                return comps.find(fx_mod_id).get();
            });
    boost::remove_erase(fx_chain_comps, nullptr);

    if (fx_chain_comps.empty())
    {
        connect(g, mixer_channel_in, mixer_channel_out);
    }
    else
    {
        connect(g, mixer_channel_in, *fx_chain_comps.front());
        connect_fx_chain(g, fx_chain_comps);
        connect(g, *fx_chain_comps.back(), mixer_channel_out);
    }
}

void
connect_mixer_input(
        audio::engine::graph& g,
        [[maybe_unused]] mixer::channels_t const& channels,
        external_audio::devices_t const& device_buses,
        component_map const& comps,
        std::span<audio::engine::input_processor> const input_procs,
        mixer::channel const& mixer_channel,
        audio::engine::component& mixer_channel_in)
{
    std::visit(
            boost::hof::match(
                    [&](external_audio::device_id const device_id) {
                        external_audio::device const& device =
                                device_buses[device_id];

                        if (device.channels.left != npos)
                        {
                            g.audio.insert(
                                    {.proc = input_procs[device.channels.left],
                                     .port = 0},
                                    mixer_channel_in.inputs()[0]);
                        }

                        if (mixer_channel.bus_type == audio::bus_type::stereo &&
                            device.channels.right != npos)
                        {
                            g.audio.insert(
                                    {.proc = input_procs[device.channels.right],
                                     .port = 0},
                                    mixer_channel_in.inputs()[1]);
                        }
                    },
                    [&](mixer::channel_id const src_channel_id) {
                        BOOST_ASSERT(channels.contains(src_channel_id));

                        audio::engine::component* const
                                source_mixer_channel_out =
                                        comps.find(mixer_output_key{
                                                           src_channel_id})
                                                .get();
                        BOOST_ASSERT(source_mixer_channel_out);

                        audio::engine::connect(
                                g,
                                *source_mixer_channel_out,
                                mixer_channel_in);
                    },
                    [](auto const&) {}),
            mixer_channel.in);
}

void
connect_mixer_output(
        audio::engine::graph& g,
        mixer::channels_t const& mixer_channels,
        external_audio::devices_t const& external_audio_devices,
        component_map const& comps,
        std::span<audio::engine::output_processor> const output_procs,
        std::span<processor_ptr> const output_clip_procs,
        mixer::io_address_t const& mixer_channel_out,
        audio::engine::component& mixer_channel_out_comp)
{
    std::visit(
            boost::hof::match(
                    [&](external_audio::device_id const device_id) {
                        external_audio::device const& device =
                                external_audio_devices[device_id];

                        auto get_clip_proc = [&](std::size_t const ch)
                                -> audio::engine::processor& {
                            auto& clip_proc = output_clip_procs[ch];
                            if (!clip_proc)
                            {
                                clip_proc = audio::engine::make_clip_processor(
                                        -1.f,
                                        1.f,
                                        std::to_string(ch));

                                g.audio.insert(
                                        {.proc = *clip_proc, .port = 0},
                                        {.proc = output_procs[ch], .port = 0});
                            }

                            return *clip_proc;
                        };

                        if (auto const ch = device.channels.left; ch != npos)
                        {
                            g.audio.insert(
                                    mixer_channel_out_comp.outputs()[0],
                                    {.proc = get_clip_proc(ch), .port = 0});
                        }

                        if (auto const ch = device.channels.right; ch != npos)
                        {
                            g.audio.insert(
                                    mixer_channel_out_comp.outputs()[1],
                                    {.proc = get_clip_proc(ch), .port = 0});
                        }
                    },
                    [&](mixer::channel_id const dst_channel_id) {
                        mixer::channel const& dst_channel =
                                mixer_channels[dst_channel_id];

                        if (std::holds_alternative<default_t>(dst_channel.in))
                        {
                            auto* const dst_mb_in =
                                    comps.find(mixer_input_key{
                                                       dst_channel_id,
                                                       dst_channel.in})
                                            .get();
                            BOOST_ASSERT(dst_mb_in);

                            audio::engine::connect(
                                    g,
                                    mixer_channel_out_comp,
                                    *dst_mb_in);
                        }
                    },
                    [](auto const&) {}),
            mixer_channel_out);
}

auto
make_graph(
        component_map const& comps,
        mixer::channels_t const& channels,
        external_audio::devices_t const& device_buses,
        std::span<audio::engine::input_processor> const input_procs,
        std::span<audio::engine::output_processor> const output_procs,
        std::span<processor_ptr> const output_clip_procs)
{
    audio::engine::graph g;

    for (auto const& [mixer_channel_id, mixer_channel] : channels)
    {
        audio::engine::component* mixer_channel_in =
                comps.find(mixer_input_key{mixer_channel_id, mixer_channel.in})
                        .get();
        audio::engine::component* mixer_channel_out =
                comps.find(mixer_output_key{mixer_channel_id}).get();

        BOOST_ASSERT(mixer_channel_in);
        BOOST_ASSERT(mixer_channel_out);

        mixer_channel_in->connect(g);
        mixer_channel_out->connect(g);

        connect_mixer_input(
                g,
                channels,
                device_buses,
                comps,
                input_procs,
                mixer_channel,
                *mixer_channel_in);

        connect_mixer_channel_with_fx_chain(
                g,
                comps,
                *mixer_channel_in,
                mixer_channel.fx_chain,
                *mixer_channel_out);

        connect_mixer_output(
                g,
                channels,
                device_buses,
                comps,
                output_procs,
                output_clip_procs,
                mixer_channel.out,
                *mixer_channel_out);

        for (auto const& [aux, aux_send] : *mixer_channel.aux_sends)
        {
            audio::engine::component* mixer_channel_aux_send =
                    comps.find(mixer_aux_send_key{mixer_channel_id, aux}).get();

            if (mixer_channel_aux_send)
            {
                mixer_channel_aux_send->connect(g);

                audio::engine::connect(
                        g,
                        *mixer_channel_out,
                        *mixer_channel_aux_send);

                connect_mixer_output(
                        g,
                        channels,
                        device_buses,
                        comps,
                        output_procs,
                        output_clip_procs,
                        aux,
                        *mixer_channel_aux_send);
            }
        }
    }

    return g;
}

void
connect_midi(
        audio::engine::graph& g,
        processor_map const& procs,
        audio::engine::processor* midi_learn_output_proc,
        parameter_processor_factory const& param_procs,
        midi_assignments_map const& assignments)
{
    auto* const midi_in_proc = procs.find(engine_processors::midi_input).get();
    if (!midi_in_proc)
    {
        return;
    }

    if (auto* const midi_learn_proc =
                procs.find(engine_processors::midi_learn).get())
    {
        BOOST_ASSERT(midi_learn_output_proc);

        g.event.insert({*midi_in_proc, 0}, {*midi_learn_proc, 0});
        g.event.insert({*midi_learn_proc, 0}, {*midi_learn_output_proc, 0});

        if (auto* const midi_assign_proc =
                    procs.find(engine_processors::midi_assign).get())
        {
            g.event.insert({*midi_learn_proc, 1}, {*midi_assign_proc, 0});
        }
    }
    else if (
            auto* const midi_assign_proc =
                    procs.find(engine_processors::midi_assign).get())
    {
        g.event.insert({*midi_in_proc, 0}, {*midi_assign_proc, 0});
    }

    if (auto* const midi_assign_proc =
                procs.find(engine_processors::midi_assign).get())
    {
        std::size_t out_index{};
        for (auto const& [id, assignment] : assignments)
        {
            std::visit(
                    [&](auto const& param_id) {
                        std::tuple const proc_id{param_id, assignment};
                        auto midi_conv_proc = procs.find(proc_id);
                        BOOST_ASSERT(midi_conv_proc);

                        g.event.insert(
                                {*midi_assign_proc, out_index++},
                                {*midi_conv_proc, 0});

                        if (auto param_proc =
                                    param_procs.find_processor(param_id))
                        {
                            g.event.insert(
                                    {*midi_conv_proc, 0},
                                    {*param_proc, 0});
                        }
                    },
                    id);
        }
    }
}

void
connect_solo_groups(
        audio::engine::graph& g,
        component_map const& comps,
        solo_groups_t const& solo_groups)
{
    auto solo_switch = comps.find(solo_group_key{});
    BOOST_ASSERT(solo_switch);

    solo_switch->connect(g);

    BOOST_ASSERT(solo_groups.size() == solo_switch->event_outputs().size());
    std::size_t index{};
    for (auto const& id : solo_groups | std::views::keys)
    {
        auto mix_out = comps.find(mixer_output_key{.channel_id = id});
        BOOST_ASSERT(mix_out);

        g.event.insert(
                solo_switch->event_outputs()[index],
                mix_out->event_inputs()[0]);

        ++index;
    }
}

template <class Processor>
auto
make_io_processors(std::size_t const num_channels)
{
    return algorithm::transform_to_vector(
            range::iota(num_channels),
            [](std::size_t const i) { return Processor(std::to_string(i)); });
}

} // namespace

struct audio_engine::impl
{
    impl(audio::sample_rate const sr,
         std::span<thread::worker> const workers,
         std::size_t num_device_input_channels,
         std::size_t num_device_output_channels)
        : sample_rate(sr)
        , worker_threads(workers)
        , input_procs(make_io_processors<audio::engine::input_processor>(
                  num_device_input_channels))
        , output_procs(make_io_processors<audio::engine::output_processor>(
                  num_device_output_channels))
        , output_clip_procs(
                  std::vector<processor_ptr>(num_device_output_channels))
    {
    }

    audio::sample_rate sample_rate;

    audio::engine::process process;
    std::span<thread::worker> worker_threads;

    std::vector<audio::engine::input_processor> input_procs;
    std::vector<audio::engine::output_processor> output_procs;

    std::vector<processor_ptr> output_clip_procs;
    std::vector<processor_ptr> mixer_procs;
    value_io_processor_ptr<midi::external_event> midi_learn_output_proc;

    processor_map procs;
    component_map comps;

    parameter_processor_factory param_procs;
    processors::stream_processor_factory stream_procs;

    audio::engine::graph graph;
};

audio_engine::audio_engine(
        std::span<thread::worker> const workers,
        audio::sample_rate const sample_rate,
        unsigned const num_device_input_channels,
        unsigned const num_device_output_channels)
    : m_impl(make_pimpl<impl>(
              sample_rate,
              workers,
              num_device_input_channels,
              num_device_output_channels))
{
}

template <class P>
void
audio_engine::set_parameter_value(
        parameter::id_t<P> const id,
        typename P::value_type const& value) const
{
    m_impl->param_procs.set(id, value);
}

template void
audio_engine::set_parameter_value(bool_parameter_id, bool const&) const;

template void
audio_engine::set_parameter_value(float_parameter_id, float const&) const;

template void
audio_engine::set_parameter_value(int_parameter_id, int const&) const;

template <class P>
auto
audio_engine::get_parameter_update(parameter::id_t<P> const id) const
        -> std::optional<typename P::value_type>
{
    using value_type = typename P::value_type;
    std::optional<value_type> result;
    m_impl->param_procs.consume(id, [&result](value_type const& lvl) {
        result = lvl;
    });
    return result;
}

template auto audio_engine::get_parameter_update(bool_parameter_id) const
        -> std::optional<bool>;

template auto audio_engine::get_parameter_update(float_parameter_id) const
        -> std::optional<float>;

template auto audio_engine::get_parameter_update(int_parameter_id) const
        -> std::optional<int>;

auto
audio_engine::get_learned_midi() const -> std::optional<midi::external_event>
{
    std::optional<midi::external_event> result;

    if (m_impl->midi_learn_output_proc)
    {
        m_impl->midi_learn_output_proc->consume(
                [&result](midi::external_event const& ev) { result = ev; });
    }

    return result;
}

auto
audio_engine::get_stream(audio_stream_id const id) const -> audio_stream_buffer
{
    if (auto proc = m_impl->stream_procs.find_processor(id))
    {
        return audio_stream_buffer{proc->consume()};
    }

    return audio_stream_buffer{};
}

bool
audio_engine::rebuild(
        state const& st,
        fx::simple_ladspa_processor_factory const& ladspa_fx_proc_factory,
        std::unique_ptr<midi::input_event_handler> midi_in)
{
    component_map comps;

    make_mixer_components(
            comps,
            m_impl->comps,
            m_impl->sample_rate,
            st.strings,
            st.mixer_state.channels,
            m_impl->param_procs,
            m_impl->stream_procs);
    make_fx_chain_components(
            comps,
            m_impl->comps,
            st.fx_modules,
            st.params,
            m_impl->param_procs,
            m_impl->stream_procs,
            ladspa_fx_proc_factory,
            m_impl->sample_rate);
    auto const solo_groups = runtime::solo_groups(st.mixer_state.channels);
    make_solo_group_components(comps, solo_groups, m_impl->param_procs);

    processor_map procs;

    bool const midi_learn = static_cast<bool>(st.midi_learning);
    make_midi_processors(std::move(midi_in), midi_learn, procs);
    make_midi_assignment_processors(
            st.midi_assignments,
            st.params,
            procs,
            m_impl->procs);
    auto midi_learn_output_proc =
            midi_learn ? std::make_unique<audio::engine::value_io_processor<
                                 midi::external_event>>("midi_learned")
                       : nullptr;

    std::vector<processor_ptr> output_clip_procs(
            m_impl->output_clip_procs.size());

    auto new_graph = make_graph(
            comps,
            st.mixer_state.channels,
            st.external_audio_state.devices,
            m_impl->input_procs,
            m_impl->output_procs,
            output_clip_procs);

    connect_midi(
            new_graph,
            procs,
            midi_learn_output_proc.get(),
            m_impl->param_procs,
            st.midi_assignments);

    connect_solo_groups(new_graph, comps, solo_groups);

    auto [final_graph, mixers] = audio::engine::finalize_graph(new_graph);

    m_impl->param_procs.initialize([&st](auto const id) {
        auto const* const desc = st.params.find(id);
        return desc ? std::optional{desc->value.get()} : std::nullopt;
    });

    if (!m_impl->process.swap_executor(
                audio::engine::graph_to_dag(final_graph)
                        .make_runnable(m_impl->worker_threads)))
    {
        return false;
    }

    m_impl->graph = std::move(final_graph);
    m_impl->output_clip_procs = std::move(output_clip_procs);
    m_impl->mixer_procs = std::move(mixers);
    m_impl->midi_learn_output_proc = std::move(midi_learn_output_proc);
    m_impl->procs = std::move(procs);
    m_impl->comps = std::move(comps);

    m_impl->param_procs.clear_expired();
    m_impl->stream_procs.clear_expired();

    {
        std::ofstream os("graph.dot");
        audio::engine::export_graph_as_dot(new_graph, os) << std::endl;
    }

    {
        std::ofstream os("final_graph.dot");
        audio::engine::export_graph_as_dot(m_impl->graph, os) << std::endl;
    }

    return true;
}

void
audio_engine::init_process(
        std::span<audio::pcm_input_buffer_converter const> const in_conv,
        std::span<audio::pcm_output_buffer_converter const> const out_conv)
{
    BOOST_ASSERT(m_impl->input_procs.size() == in_conv.size());
    for (std::size_t const i : range::indices(in_conv))
    {
        m_impl->input_procs[i].set_input(in_conv[i]);
    }

    BOOST_ASSERT(m_impl->output_procs.size() == out_conv.size());
    for (std::size_t const i : range::indices(out_conv))
    {
        m_impl->output_procs[i].set_output(out_conv[i]);
    }
}

void
audio_engine::process(std::size_t const buffer_size) noexcept
{
    m_impl->process(buffer_size);
}

} // namespace piejam::runtime
