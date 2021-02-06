// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/audio_engine.h>

#include <piejam/algorithm/concat.h>
#include <piejam/algorithm/for_each_adjacent.h>
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
#include <piejam/audio/engine/processor_map.h>
#include <piejam/audio/engine/value_io_processor.h>
#include <piejam/audio/engine/value_sink_processor.h>
#include <piejam/functional/overload.h>
#include <piejam/midi/event.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/components/make_fx.h>
#include <piejam/runtime/components/mixer_bus.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameter/maps_collection.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/runtime/processors/midi_assignment_processor.h>
#include <piejam/runtime/processors/midi_input_processor.h>
#include <piejam/runtime/processors/midi_learn_processor.h>
#include <piejam/runtime/processors/midi_to_parameter_processor.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/worker.h>

#include <fmt/format.h>

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

using processor_ptr = std::unique_ptr<audio::engine::processor>;
using component_ptr = std::unique_ptr<audio::engine::component>;

template <class T>
using value_input_processor_ptr =
        std::unique_ptr<audio::engine::value_io_processor<T>>;

template <class T>
using value_output_processor_ptr =
        std::unique_ptr<audio::engine::value_io_processor<T>>;

struct fx_module_component_mapping
{
    fx::module_id fx_mod_id;
    component_ptr component;
};

using fx_chain_components = std::vector<fx_module_component_mapping>;

using fx_chains_map = std::unordered_map<mixer::bus_id, fx_chain_components>;

struct mixer_bus_component_mapping
{
    mixer::bus_id bus_id;
    component_ptr input;
    component_ptr output;
};

using mixer_bus_components = std::vector<mixer_bus_component_mapping>;

auto
make_mixer_bus_vector(
        mixer_bus_components& prev_buses,
        unsigned const samplerate,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& bus_ids,
        parameter_processor_factory& param_procs) -> mixer_bus_components
{
    mixer_bus_components result;
    result.reserve(bus_ids.size());

    for (auto const& id : bus_ids)
    {
        if (auto it = std::ranges::find(
                    prev_buses,
                    id,
                    &mixer_bus_component_mapping::bus_id);
            it != prev_buses.end())
        {
            result.emplace_back(std::move(*it));
            prev_buses.erase(it);
        }
        else
        {
            result.emplace_back(
                    id,
                    components::make_mixer_bus_input(*buses[id], param_procs),
                    components::make_mixer_bus_output(
                            samplerate,
                            id,
                            *buses[id],
                            param_procs));
        }
    }

    return result;
}

auto
make_fx_chains_map(
        fx_chains_map& prev_fx_chains,
        fx::modules_t const& fx_modules,
        fx::parameters_t const& fx_params,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& all_ids,
        parameter_processor_factory& param_procs,
        fx::ladspa_processor_factory const& ladspa_fx_proc_factory)
        -> fx_chains_map
{
    auto get_fx_param_name =
            [&fx_params](fx::parameter_id id) -> std::string_view {
        return *fx_params.at(id).name;
    };

    fx_chains_map fx_chains;

    for (mixer::bus_id const bus_id : all_ids)
    {
        fx_chain_components& fx_chain_comps = fx_chains[bus_id];

        if (auto it = prev_fx_chains.find(bus_id); it != prev_fx_chains.end())
        {
            for (fx::module_id const fx_mod_id : *buses[bus_id]->fx_chain)
            {
                if (auto it_fx_mod = std::ranges::find(
                            it->second,
                            fx_mod_id,
                            &fx_module_component_mapping::fx_mod_id);
                    it_fx_mod != it->second.end())
                {
                    fx_chain_comps.emplace_back(std::move(*it_fx_mod));
                    it->second.erase(it_fx_mod);
                }
                else
                {
                    fx::module const* const fx_mod = fx_modules[fx_mod_id];
                    BOOST_ASSERT(fx_mod);
                    auto comp = components::make_fx(
                            *fx_mod,
                            get_fx_param_name,
                            ladspa_fx_proc_factory,
                            param_procs);
                    if (comp)
                    {
                        fx_chain_comps.emplace_back(fx_mod_id, std::move(comp));
                    }
                }
            }
        }
        else
        {
            for (fx::module_id const fx_mod_id : *buses[bus_id]->fx_chain)
            {
                fx::module const* const fx_mod = fx_modules[fx_mod_id];
                BOOST_ASSERT(fx_mod);
                auto comp = components::make_fx(
                        *fx_mod,
                        get_fx_param_name,
                        ladspa_fx_proc_factory,
                        param_procs);
                if (comp)
                {
                    fx_chain_comps.emplace_back(fx_mod_id, std::move(comp));
                }
            }
        }
    }

    return fx_chains;
}

auto
make_midi_processors(
        std::unique_ptr<midi::input_event_handler> midi_in,
        bool const midi_learning,
        audio::engine::processor_map& procs)
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
        audio::engine::processor_map& procs,
        audio::engine::processor_map& prev_procs)
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
                    auto const* param = params.get_parameter(param_id);
                    BOOST_ASSERT(param);

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
                                        *param));
                    }
                },
                id);
    }
}

auto
connect_fx_chain(
        audio::engine::graph& g,
        fx_chain_components const& fx_chain,
        std::vector<processor_ptr>& mixer_procs)
{
    for (auto const& comp : fx_chain)
        comp.component->connect(g);

    algorithm::for_each_adjacent(
            fx_chain,
            [&g, &mixer_procs](
                    fx_module_component_mapping const& l_comp,
                    fx_module_component_mapping const& r_comp) {
                connect_stereo_components(
                        g,
                        *l_comp.component,
                        *r_comp.component,
                        mixer_procs);
            });
}

auto
connect_mixer_bus_with_fx_chain(
        audio::engine::graph& g,
        audio::engine::component& mb_in,
        fx_chain_components const& fx_chain,
        audio::engine::component& mb_out,
        std::vector<processor_ptr>& mixer_procs)
{
    if (fx_chain.empty())
    {
        connect_stereo_components(g, mb_in, mb_out, mixer_procs);
    }
    else
    {
        connect_stereo_components(
                g,
                mb_in,
                *fx_chain.front().component,
                mixer_procs);
        connect_fx_chain(g, fx_chain, mixer_procs);
        connect_stereo_components(
                g,
                *fx_chain.back().component,
                mb_out,
                mixer_procs);
    }
}

auto
make_graph(
        mixer::state const& mixer_state,
        audio::engine::processor& input_proc,
        audio::engine::processor& output_proc,
        mixer_bus_components const& input_buses,
        mixer_bus_components const& output_buses,
        fx_chains_map const& fx_chains,
        audio::engine::processor& input_solo_index,
        std::vector<processor_ptr>& mixer_procs)
{
    audio::engine::graph g;

    BOOST_ASSERT(mixer_state.inputs->size() == input_buses.size());
    for (std::size_t idx = 0; idx < mixer_state.inputs->size(); ++idx)
    {
        auto const& bus = mixer::get_bus<io_direction::input>(mixer_state, idx);
        auto const& [id, mb_in, mb_out] = input_buses[idx];

        mb_in->connect(g);
        mb_out->connect(g);

        connect_mixer_bus_with_fx_chain(
                g,
                *mb_in,
                fx_chains.at(id),
                *mb_out,
                mixer_procs);

        if (bus.device_channels.left != npos)
            g.add_wire(
                    {input_proc, bus.device_channels.left},
                    mb_in->inputs()[0]);

        if (bus.device_channels.right != npos)
            g.add_wire(
                    {input_proc, bus.device_channels.right},
                    mb_in->inputs()[1]);

        g.add_event_wire({input_solo_index, 0}, mb_out->event_inputs()[0]);
    }

    BOOST_ASSERT(mixer_state.outputs->size() == output_buses.size());
    for (std::size_t idx = 0; idx < mixer_state.outputs->size(); ++idx)
    {
        auto const& bus =
                mixer::get_bus<io_direction::output>(mixer_state, idx);
        auto const& [id, mb_in, mb_out] = output_buses[idx];

        mb_in->connect(g);
        mb_out->connect(g);

        connect_mixer_bus_with_fx_chain(
                g,
                *mb_in,
                fx_chains.at(id),
                *mb_out,
                mixer_procs);

        if (bus.device_channels.left != npos)
            connect(g,
                    mb_out->outputs()[0],
                    {output_proc, bus.device_channels.left},
                    mixer_procs);

        if (bus.device_channels.right != npos)
            connect(g,
                    mb_out->outputs()[1],
                    {output_proc, bus.device_channels.right},
                    mixer_procs);
    }

    for (auto const& [id, out_mb_in, out_mb_out] : output_buses)
    {
        for (auto const& [id, in_mb_in, in_mb_out] : input_buses)
        {
            connect_stereo_components(g, *in_mb_out, *out_mb_in, mixer_procs);
        }
    }

    return g;
}

void
connect_midi(
        audio::engine::graph& g,
        audio::engine::processor_map const& procs,
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

} // namespace

struct audio_engine::impl
{
    impl(std::span<thread::configuration const> const& wt_configs,
         unsigned num_device_input_channels,
         unsigned num_device_output_channels)
        : process(num_device_input_channels, num_device_output_channels)
        , worker_threads(wt_configs.begin(), wt_configs.end())
    {
    }

    audio::engine::process process;
    std::vector<thread::worker> worker_threads;

    mixer_bus_components input_buses;
    mixer_bus_components output_buses;
    fx_chains_map fx_chains;
    value_input_processor_ptr<mixer::bus_id> input_solo_index_proc;
    std::vector<processor_ptr> mixer_procs;
    value_output_processor_ptr<midi::external_event> midi_learn_output_proc;

    audio::engine::processor_map procs;

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

void
audio_engine::set_input_solo(mixer::bus_id const& id)
{
    m_impl->input_solo_index_proc->set(id);
}

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
        mixer::state const& mixer_state,
        fx::modules_t const& fx_modules,
        fx::parameters_t const& fx_params,
        parameter_maps const& params,
        fx::ladspa_processor_factory const& ladspa_fx_proc_factory,
        std::unique_ptr<midi::input_event_handler> midi_in,
        bool midi_learn,
        midi_assignments_map const& assignments)
{
    auto input_buses = make_mixer_bus_vector(
            m_impl->input_buses,
            m_samplerate,
            mixer_state.buses,
            mixer_state.inputs,
            m_impl->param_procs);
    auto output_buses = make_mixer_bus_vector(
            m_impl->output_buses,
            m_samplerate,
            mixer_state.buses,
            mixer_state.outputs,
            m_impl->param_procs);
    auto fx_chains = make_fx_chains_map(
            m_impl->fx_chains,
            fx_modules,
            fx_params,
            mixer_state.buses,
            algorithm::concat<mixer::bus_list_t>(
                    mixer_state.inputs.get(),
                    mixer_state.outputs.get()),
            m_impl->param_procs,
            ladspa_fx_proc_factory);

    m_impl->param_procs.initialize(params);

    audio::engine::processor_map procs;

    auto input_solo_index_proc =
            std::make_unique<audio::engine::value_io_processor<mixer::bus_id>>(
                    mixer_state.input_solo_id,
                    "input_solo_index");

    make_midi_processors(std::move(midi_in), midi_learn, procs);
    make_midi_assignment_processors(assignments, params, procs, m_impl->procs);
    auto midi_learn_output_proc =
            midi_learn ? std::make_unique<audio::engine::value_io_processor<
                                 midi::external_event>>("midi_learned")
                       : nullptr;

    std::vector<processor_ptr> mixers;

    auto new_graph = make_graph(
            mixer_state,
            m_impl->process.input(),
            m_impl->process.output(),
            input_buses,
            output_buses,
            fx_chains,
            *input_solo_index_proc,
            mixers);

    connect_midi(
            new_graph,
            procs,
            midi_learn_output_proc.get(),
            m_impl->param_procs,
            assignments);

    audio::engine::bypass_event_identity_processors(new_graph);

    if (!m_impl->process.swap_executor(
                audio::engine::graph_to_dag(new_graph).make_runnable(
                        m_impl->worker_threads)))
        return false;

    m_impl->graph = std::move(new_graph);
    m_impl->input_buses = std::move(input_buses);
    m_impl->output_buses = std::move(output_buses);
    m_impl->fx_chains = std::move(fx_chains);
    m_impl->input_solo_index_proc = std::move(input_solo_index_proc);
    m_impl->mixer_procs = std::move(mixers);
    m_impl->midi_learn_output_proc = std::move(midi_learn_output_proc);
    m_impl->procs = std::move(procs);

    m_impl->param_procs.clear_expired();

    std::ofstream("graph.dot")
            << audio::engine::export_graph_as_dot(m_impl->graph) << std::endl;

    return true;
}

void
audio_engine::operator()(
        range::table_view<float const> const& in_audio,
        range::table_view<float> const& out_audio) noexcept
{
    m_impl->process(in_audio, out_audio);
}

} // namespace piejam::runtime
