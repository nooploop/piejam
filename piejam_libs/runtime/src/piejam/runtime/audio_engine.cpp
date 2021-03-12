// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/audio_engine.h>

#include <piejam/algorithm/for_each_adjacent.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_algorithms.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/input_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/output_processor.h>
#include <piejam/audio/engine/process.h>
#include <piejam/audio/engine/value_io_processor.h>
#include <piejam/audio/engine/value_sink_processor.h>
#include <piejam/functional/overload.h>
#include <piejam/midi/event.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/components/make_fx.h>
#include <piejam/runtime/components/mixer_channel.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/components/solo_switch.h>
#include <piejam/runtime/device_io.h>
#include <piejam/runtime/dynamic_key_object_map.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/mixer_util.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/runtime/processors/midi_assignment_processor.h>
#include <piejam/runtime/processors/midi_input_processor.h>
#include <piejam/runtime/processors/midi_learn_processor.h>
#include <piejam/runtime/processors/midi_to_parameter_processor.h>
#include <piejam/runtime/solo_group.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/worker.h>

#include <fmt/format.h>

#include <boost/range/algorithm_ext/erase.hpp>

#include <algorithm>
#include <fstream>
#include <optional>
#include <ranges>
#include <unordered_map>

namespace piejam::runtime
{

namespace
{

enum class engine_processors
{
    midi_input,
    midi_sink,
    midi_learn,
    midi_assign
};

struct mixer_input_key
{
    mixer::channel_id channel_id;
    mixer::io_address_t route;

    bool operator==(mixer_input_key const&) const noexcept = default;
};

struct mixer_output_key
{
    mixer::channel_id channel_id;

    bool operator==(mixer_output_key const&) const noexcept = default;
};

struct solo_group_key
{
    mixer::channel_id owner;

    bool operator==(solo_group_key const&) const noexcept = default;
};

using processor_map = dynamic_key_object_map<audio::engine::processor>;
using component_map = dynamic_key_object_map<audio::engine::component>;

using processor_ptr = std::unique_ptr<audio::engine::processor>;
using component_ptr = std::unique_ptr<audio::engine::component>;

template <class T>
using value_input_processor_ptr =
        std::unique_ptr<audio::engine::value_io_processor<T>>;

template <class T>
using value_output_processor_ptr =
        std::unique_ptr<audio::engine::value_io_processor<T>>;

using get_device_processor_f =
        std::function<audio::engine::processor&(std::size_t)>;

void
make_mixer_components(
        component_map& comps,
        component_map& prev_comps,
        unsigned const samplerate,
        mixer::channels_t const& channels,
        device_io::buses_t const& device_buses,
        parameter_processor_factory& param_procs)
{
    for (auto const& [id, bus] : channels)
    {
        mixer_input_key const in_key{id, bus.in};
        if (auto comp = prev_comps.remove(in_key))
        {
            comps.insert(in_key, std::move(comp));
        }
        else
        {
            audio::bus_type const bus_type =
                    mixer_channel_input_type(channels, id, device_buses);

            comps.insert(
                    in_key,
                    components::make_mixer_channel_input(
                            bus,
                            bus_type,
                            param_procs));
        }

        mixer_output_key const out_key{id};
        if (auto comp = prev_comps.remove(out_key))
        {
            comps.insert(out_key, std::move(comp));
        }
        else
        {
            comps.insert(
                    out_key,
                    components::make_mixer_channel_output(
                            bus,
                            samplerate,
                            param_procs));
        }
    }
}

void
make_solo_group_components(
        component_map& comps,
        solo_groups_t const& solo_groups,
        parameter_processor_factory& param_procs)
{
    for (auto&& [id, group] : solo_groups)
    {
        solo_group_key key{id};
        comps.insert(
                key,
                components::make_solo_switch(group, param_procs, "solo"));
    }
}

auto
make_fx_chain_components(
        component_map& comps,
        component_map& prev_comps,
        fx::modules_t const& fx_modules,
        fx::parameters_t const& fx_params,
        parameter_processor_factory& param_procs,
        fx::simple_ladspa_processor_factory const& ladspa_fx_proc_factory)
{
    auto get_fx_param_name =
            [&fx_params](fx::parameter_id id) -> std::string_view {
        return *fx_params.at(id).name;
    };

    for (auto const& [fx_mod_id, fx_mod] : fx_modules)
    {
        if (auto fx_comp = prev_comps.remove(fx_mod_id))
        {
            comps.insert(fx_mod_id, std::move(fx_comp));
        }
        else
        {
            auto comp = components::make_fx(
                    fx_mod,
                    get_fx_param_name,
                    ladspa_fx_proc_factory,
                    param_procs);
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
        else
        {
            procs.insert(
                    engine_processors::midi_sink,
                    std::make_unique<audio::engine::value_sink_processor<
                            midi::external_event>>("ext_midi_sink"));
        }
    }
}

auto
make_midi_assignment_processors(
        midi_assignments_map const& assignments,
        parameter_maps const& params,
        processor_map& procs,
        processor_map& prev_procs)
{
    if (assignments.empty())
        return;

    procs.insert(
            engine_processors::midi_assign,
            processors::make_midi_assignment_processor(assignments));

    for (auto const& [id, ass] : assignments)
    {
        std::visit(
                [&](auto const& param_id) {
                    auto const& param = get_parameter(params, param_id);

                    std::tuple const proc_id{param_id, ass};
                    if (auto proc = prev_procs.remove(proc_id))
                    {
                        procs.insert(proc_id, std::move(proc));
                    }
                    else
                    {
                        procs.insert(
                                proc_id,
                                processors::make_midi_cc_to_parameter_processor(
                                        param));
                    }
                },
                id);
    }
}

auto
connect_fx_chain(
        audio::engine::graph& g,
        std::vector<audio::engine::component*> const& fx_chain_comps,
        std::vector<processor_ptr>& mixer_procs)
{
    for (auto comp : fx_chain_comps)
        comp->connect(g);

    algorithm::for_each_adjacent(
            fx_chain_comps,
            [&](auto const l_comp, auto const r_comp) {
                connect_stereo_components(g, *l_comp, *r_comp, mixer_procs);
            });
}

auto
connect_mixer_channel_with_fx_chain(
        audio::engine::graph& g,
        component_map const& comps,
        audio::engine::component& mb_in,
        fx::chain_t const& fx_chain,
        audio::engine::component& mb_out,
        std::vector<processor_ptr>& mixer_procs)
{
    auto fx_chain_comps = algorithm::transform_to_vector(
            fx_chain,
            [&comps](fx::module_id fx_mod_id) {
                return comps.find(fx_mod_id);
            });
    boost::remove_erase(fx_chain_comps, nullptr);

    if (fx_chain_comps.empty())
    {
        connect_stereo_components(g, mb_in, mb_out, mixer_procs);
    }
    else
    {
        connect_stereo_components(
                g,
                mb_in,
                *fx_chain_comps.front(),
                mixer_procs);
        connect_fx_chain(g, fx_chain_comps, mixer_procs);
        connect_stereo_components(
                g,
                *fx_chain_comps.back(),
                mb_out,
                mixer_procs);
    }
}

void
connect_mixer_input(
        audio::engine::graph& g,
        mixer::channels_t const& channels,
        device_io::buses_t const& device_buses,
        component_map const& comps,
        get_device_processor_f const& get_input_proc,
        std::vector<processor_ptr>& mixer_procs,
        mixer::channel const& bus,
        audio::engine::component& mb_in)
{
    boost::ignore_unused(channels);
    std::visit(
            overload{
                    [](std::nullptr_t) {},
                    [&](device_io::bus_id const device_bus_id) {
                        device_io::bus const& device_bus =
                                device_buses[device_bus_id];

                        if (device_bus.channels.left != npos)
                            g.add_wire(
                                    {.proc = get_input_proc(
                                             device_bus.channels.left),
                                     .port = 0},
                                    mb_in.inputs()[0]);

                        if (device_bus.channels.right != npos)
                            g.add_wire(
                                    {.proc = get_input_proc(
                                             device_bus.channels.right),
                                     .port = 0},
                                    mb_in.inputs()[1]);
                    },
                    [&](mixer::channel_id const src_channel_id) {
                        BOOST_ASSERT(channels.contains(src_channel_id));

                        auto* const source_mb_out =
                                comps.find(mixer_output_key{src_channel_id});
                        BOOST_ASSERT(source_mb_out);

                        audio::engine::connect_stereo_components(
                                g,
                                *source_mb_out,
                                mb_in,
                                mixer_procs);
                    },
                    [](boxed_string const&) {}},
            bus.in);
}

void
connect_mixer_output(
        audio::engine::graph& g,
        mixer::channels_t const& channels,
        device_io::buses_t const& device_buses,
        component_map const& comps,
        get_device_processor_f const& get_output_proc,
        std::vector<processor_ptr>& mixer_procs,
        mixer::channel const& bus,
        audio::engine::component& mb_out)
{
    std::visit(
            overload{
                    [](std::nullptr_t) {},
                    [&](device_io::bus_id const device_bus_id) {
                        device_io::bus const& device_bus =
                                device_buses[device_bus_id];

                        if (device_bus.channels.left != npos)
                            connect(g,
                                    mb_out.outputs()[0],
                                    {.proc = get_output_proc(
                                             device_bus.channels.left),
                                     .port = 0},
                                    mixer_procs);

                        if (device_bus.channels.right != npos)
                            connect(g,
                                    mb_out.outputs()[1],
                                    {.proc = get_output_proc(
                                             device_bus.channels.right),
                                     .port = 0},
                                    mixer_procs);
                    },
                    [&](mixer::channel_id const dst_channel_id) {
                        mixer::channel const& dst_channel =
                                channels[dst_channel_id];

                        if (std::holds_alternative<std::nullptr_t>(
                                    dst_channel.in))
                        {
                            auto* const dst_mb_in = comps.find(mixer_input_key{
                                    dst_channel_id,
                                    dst_channel.in});
                            BOOST_ASSERT(dst_mb_in);

                            audio::engine::connect_stereo_components(
                                    g,
                                    mb_out,
                                    *dst_mb_in,
                                    mixer_procs);
                        }
                    },
                    [](boxed_string const&) {}},
            bus.out);
}

auto
make_graph(
        component_map const& comps,
        mixer::channels_t const& channels,
        device_io::buses_t const& device_buses,
        get_device_processor_f const& get_input_proc,
        get_device_processor_f const& get_output_proc,
        std::vector<processor_ptr>& mixer_procs)
{
    audio::engine::graph g;

    for (auto const& [id, bus] : channels)
    {
        auto* mb_in = comps.find(mixer_input_key{id, bus.in});
        auto* mb_out = comps.find(mixer_output_key{id});

        BOOST_ASSERT(mb_in);
        BOOST_ASSERT(mb_out);

        mb_in->connect(g);
        mb_out->connect(g);

        connect_mixer_input(
                g,
                channels,
                device_buses,
                comps,
                get_input_proc,
                mixer_procs,
                bus,
                *mb_in);

        connect_mixer_channel_with_fx_chain(
                g,
                comps,
                *mb_in,
                bus.fx_chain,
                *mb_out,
                mixer_procs);

        connect_mixer_output(
                g,
                channels,
                device_buses,
                comps,
                get_output_proc,
                mixer_procs,
                bus,
                *mb_out);
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
    auto* const midi_in_proc = procs.find(engine_processors::midi_input);
    if (!midi_in_proc)
        return;

    if (auto* const midi_learn_proc = procs.find(engine_processors::midi_learn))
    {
        BOOST_ASSERT(midi_learn_output_proc);

        g.add_event_wire({*midi_in_proc, 0}, {*midi_learn_proc, 0});
        g.add_event_wire({*midi_learn_proc, 0}, {*midi_learn_output_proc, 0});

        if (auto* const midi_assign_proc =
                    procs.find(engine_processors::midi_assign))
            g.add_event_wire({*midi_learn_proc, 1}, {*midi_assign_proc, 0});
    }
    else if (
            auto* const midi_assign_proc =
                    procs.find(engine_processors::midi_assign))
    {
        g.add_event_wire({*midi_in_proc, 0}, {*midi_assign_proc, 0});
    }
    else
    {
        auto* const midi_sink = procs.find(engine_processors::midi_sink);
        BOOST_ASSERT(midi_sink);
        g.add_event_wire({*midi_in_proc, 0}, {*midi_sink, 0});
    }

    if (auto* const midi_assign_proc =
                procs.find(engine_processors::midi_assign))
    {
        std::size_t out_index{};
        for (auto const& [id, ass] : assignments)
        {
            std::visit(
                    [&](auto const& param_id) {
                        std::tuple const proc_id{param_id, ass};
                        auto proc = procs.find(proc_id);
                        BOOST_ASSERT(proc);

                        g.add_event_wire(
                                {*midi_assign_proc, out_index++},
                                {*proc, 0});

                        if (auto param_proc =
                                    param_procs.find_processor(param_id))
                            g.add_event_wire({*proc, 0}, {*param_proc, 0});
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
    for (auto const& [owner, group] : solo_groups)
    {
        auto solo_switch = comps.find(solo_group_key{owner});
        BOOST_ASSERT(solo_switch);

        solo_switch->connect(g);

        BOOST_ASSERT(group.size() == solo_switch->event_outputs().size());
        for (std::size_t const index : range::indices(group))
        {
            auto const& [param, member] = group[index];
            auto mix_out = comps.find(mixer_output_key{.channel_id = member});
            BOOST_ASSERT(mix_out);

            g.add_event_wire(
                    solo_switch->event_outputs()[index],
                    mix_out->event_inputs()[0]);
        }
    }
}

template <class Processor>
auto
make_io_processors(std::size_t num_channels)
{
    return algorithm::transform_to_vector(
            range::iota(num_channels),
            [](std::size_t const i) { return Processor(std::to_string(i)); });
}

} // namespace

struct audio_engine::impl
{
    impl(std::span<thread::configuration const> const& wt_configs,
         unsigned num_device_input_channels,
         unsigned num_device_output_channels)
        : worker_threads(wt_configs.begin(), wt_configs.end())
        , input_procs(make_io_processors<audio::engine::input_processor>(
                  num_device_input_channels))
        , output_procs(make_io_processors<audio::engine::output_processor>(
                  num_device_output_channels))
    {
    }

    audio::engine::process process;
    std::vector<thread::worker> worker_threads;

    std::vector<audio::engine::input_processor> input_procs;
    std::vector<audio::engine::output_processor> output_procs;

    std::vector<processor_ptr> mixer_procs;
    value_output_processor_ptr<midi::external_event> midi_learn_output_proc;

    processor_map procs;
    component_map comps;

    parameter_processor_factory param_procs;

    audio::engine::graph graph;
};

audio_engine::audio_engine(
        std::span<thread::configuration const> const& wt_configs,
        audio::samplerate_t const samplerate,
        unsigned const num_device_input_channels,
        unsigned const num_device_output_channels)
    : m_samplerate(samplerate)
    , m_impl(std::make_unique<impl>(
              wt_configs,
              num_device_input_channels,
              num_device_output_channels))
{
}

audio_engine::~audio_engine() = default;

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
    std::optional<typename P::value_type> result;
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

template auto
        audio_engine::get_parameter_update(stereo_level_parameter_id) const
        -> std::optional<stereo_level>;

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

bool
audio_engine::rebuild(
        mixer::channels_t const& mixer_channels,
        device_io::buses_t const& device_buses,
        fx::modules_t const& fx_modules,
        fx::parameters_t const& fx_params,
        parameter_maps const& params,
        fx::simple_ladspa_processor_factory const& ladspa_fx_proc_factory,
        std::unique_ptr<midi::input_event_handler> midi_in,
        bool midi_learn,
        midi_assignments_map const& assignments)
{
    component_map comps;

    make_mixer_components(
            comps,
            m_impl->comps,
            m_samplerate,
            mixer_channels,
            device_buses,
            m_impl->param_procs);
    make_fx_chain_components(
            comps,
            m_impl->comps,
            fx_modules,
            fx_params,
            m_impl->param_procs,
            ladspa_fx_proc_factory);
    auto const solo_groups = runtime::solo_groups(mixer_channels);
    make_solo_group_components(comps, solo_groups, m_impl->param_procs);

    m_impl->param_procs.initialize([&params](auto const id) {
        return find_parameter_value(params, id);
    });

    processor_map procs;

    make_midi_processors(std::move(midi_in), midi_learn, procs);
    make_midi_assignment_processors(assignments, params, procs, m_impl->procs);
    auto midi_learn_output_proc =
            midi_learn ? std::make_unique<audio::engine::value_io_processor<
                                 midi::external_event>>("midi_learned")
                       : nullptr;

    std::vector<processor_ptr> mixers;

    auto new_graph = make_graph(
            comps,
            mixer_channels,
            device_buses,
            [this](std::size_t ch) -> audio::engine::processor& {
                return m_impl->input_procs[ch];
            },
            [this](std::size_t ch) -> audio::engine::processor& {
                return m_impl->output_procs[ch];
            },
            mixers);

    connect_midi(
            new_graph,
            procs,
            midi_learn_output_proc.get(),
            m_impl->param_procs,
            assignments);

    connect_solo_groups(new_graph, comps, solo_groups);

    audio::engine::bypass_event_identity_processors(new_graph);

    if (!m_impl->process.swap_executor(
                audio::engine::graph_to_dag(new_graph).make_runnable(
                        m_impl->worker_threads)))
        return false;

    m_impl->graph = std::move(new_graph);
    m_impl->mixer_procs = std::move(mixers);
    m_impl->midi_learn_output_proc = std::move(midi_learn_output_proc);
    m_impl->procs = std::move(procs);
    m_impl->comps = std::move(comps);

    m_impl->param_procs.clear_expired();

    std::ofstream("graph.dot")
            << audio::engine::export_graph_as_dot(m_impl->graph) << std::endl;

    return true;
}

void
audio_engine::operator()(
        std::span<audio::pcm_input_buffer_converter const> const& in_conv,
        std::span<audio::pcm_output_buffer_converter const> const& out_conv,
        std::size_t const buffer_size) noexcept
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

    m_impl->process(buffer_size);
}

} // namespace piejam::runtime
