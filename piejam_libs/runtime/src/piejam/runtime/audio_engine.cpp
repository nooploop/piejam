// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/runtime/audio_engine.h>

#include <piejam/algorithm/for_each_adjacent.h>
#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/components/level_meter.h>
#include <piejam/audio/components/pan.h>
#include <piejam/audio/components/stereo_balance.h>
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
#include <piejam/audio/engine/value_input_processor.h>
#include <piejam/audio/engine/value_output_processor.h>
#include <piejam/functional/overload.h>
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/components/fx_gain.h>
#include <piejam/runtime/components/fx_ladspa.h>
#include <piejam/runtime/components/mixer_bus.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameter/maps_collection.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/thread/configuration.h>

#include <fmt/format.h>

#include <boost/range/algorithm_ext/push_back.hpp>

#include <algorithm>
#include <fstream>
#include <optional>
#include <ranges>
#include <unordered_map>

namespace piejam::runtime
{

namespace
{

namespace ns_ae = audio::engine;

using processor_ptr = std::unique_ptr<audio::engine::processor>;
using component_ptr = std::unique_ptr<audio::engine::component>;

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

} // namespace

struct audio_engine::impl
{
    impl(unsigned num_device_input_channels,
         unsigned num_device_output_channels)
        : process(num_device_input_channels, num_device_output_channels)
    {
    }

    audio::engine::process process;

    mixer_bus_components input_buses;
    mixer_bus_components output_buses;
    fx_chains_map fx_chains;
    std::unique_ptr<audio::engine::value_input_processor<mixer::bus_id>>
            input_solo_index_proc;
    std::vector<processor_ptr> mixer_procs;

    parameter_processor_factory param_procs;

    audio::engine::graph graph;
};

static auto
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

static auto
make_fx_chains_map(
        fx_chains_map& prev_fx_chains,
        fx::modules_t const& fx_modules,
        fx::parameters_t const& fx_params,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& inputs,
        mixer::bus_list_t const& outputs,
        parameter_processor_factory& param_procs,
        fx::ladspa_processor_factory const& ladspa_fx_proc_factory)
        -> fx_chains_map
{
    mixer::bus_list_t all_ids;
    all_ids.reserve(inputs.size() + outputs.size());
    boost::push_back(all_ids, inputs);
    boost::push_back(all_ids, outputs);

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
                    BOOST_ASSERT(fx_mod);
                    std::visit(
                            overload{
                                    [&](fx::internal fx_type) {
                                        switch (fx_type)
                                        {
                                            case fx::internal::gain:
                                                fx_chain_comps.emplace_back(
                                                        fx_mod_id,
                                                        components::make_fx_gain(
                                                                *fx_mod,
                                                                param_procs));
                                        }
                                    },
                                    [&](fx::ladspa_instance_id id) {
                                        auto comp = components::make_fx_ladspa(
                                                *fx_mod,
                                                [&fx_params](
                                                        fx::parameter_id id)
                                                        -> std::string_view {
                                                    return *fx_params.at(id)
                                                                    .name;
                                                },
                                                [&]() {
                                                    return ladspa_fx_proc_factory(
                                                            id);
                                                },
                                                param_procs);
                                        if (comp)
                                        {
                                            fx_chain_comps.emplace_back(
                                                    fx_mod_id,
                                                    std::move(comp));
                                        }
                                    }},
                            fx_mod->fx_type_id);
                }
            }
        }
        else
        {
            for (fx::module_id const fx_mod_id : *buses[bus_id]->fx_chain)
            {
                fx::module const* const fx_mod = fx_modules[fx_mod_id];
                BOOST_ASSERT(fx_mod);
                std::visit(
                        overload{
                                [&](fx::internal fx_type) {
                                    switch (fx_type)
                                    {
                                        case fx::internal::gain:
                                            fx_chain_comps.emplace_back(
                                                    fx_mod_id,
                                                    components::make_fx_gain(
                                                            *fx_mod,
                                                            param_procs));
                                    }
                                },
                                [&](fx::ladspa_instance_id id) {
                                    auto comp = components::make_fx_ladspa(
                                            *fx_mod,
                                            [&fx_params](fx::parameter_id id)
                                                    -> std::string_view {
                                                return *fx_params.at(id).name;
                                            },
                                            [&]() {
                                                return ladspa_fx_proc_factory(
                                                        id);
                                            },
                                            param_procs);
                                    if (comp)
                                    {
                                        fx_chain_comps.emplace_back(
                                                fx_mod_id,
                                                std::move(comp));
                                    }
                                }},
                        fx_mod->fx_type_id);
            }
        }
    }

    return fx_chains;
}

static auto
connect_fx_chain(
        ns_ae::graph& g,
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

static auto
connect_mixer_bus_with_fx_chain(
        ns_ae::graph& g,
        ns_ae::component& mb_in,
        fx_chain_components const& fx_chain,
        ns_ae::component& mb_out,
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

static auto
make_graph(
        mixer::state const& mixer_state,
        ns_ae::processor& input_proc,
        ns_ae::processor& output_proc,
        mixer_bus_components const& input_buses,
        mixer_bus_components const& output_buses,
        fx_chains_map const& fx_chains,
        ns_ae::processor& input_solo_index,
        std::vector<processor_ptr>& mixer_procs)
{
    ns_ae::graph g;

    BOOST_ASSERT(mixer_state.inputs->size() == input_buses.size());
    for (std::size_t idx = 0; idx < mixer_state.inputs->size(); ++idx)
    {
        auto const& bus =
                mixer::get_bus<audio::bus_direction::input>(mixer_state, idx);
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
                mixer::get_bus<audio::bus_direction::output>(mixer_state, idx);
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

audio_engine::audio_engine(
        std::span<thread::configuration const> const& wt_configs,
        audio::samplerate_t const samplerate,
        unsigned const num_device_input_channels,
        unsigned const num_device_output_channels)
    : m_wt_configs(wt_configs.begin(), wt_configs.end())
    , m_samplerate(samplerate)
    , m_impl(std::make_unique<impl>(
              num_device_input_channels,
              num_device_output_channels))
{
}

audio_engine::~audio_engine() = default;

void
audio_engine::set_bool_parameter(bool_parameter_id const id, bool const value)
        const
{
    m_impl->param_procs.set(id, value);
}

void
audio_engine::set_float_parameter(
        float_parameter_id const id,
        float const value) const
{
    m_impl->param_procs.set(id, value);
}

void
audio_engine::set_int_parameter(int_parameter_id const id, int const value)
        const
{
    m_impl->param_procs.set(id, value);
}

void
audio_engine::set_input_solo(mixer::bus_id const& id)
{
    m_impl->input_solo_index_proc->set(id);
}

auto
audio_engine::get_level(stereo_level_parameter_id const id) const
        -> std::optional<stereo_level>
{
    std::optional<stereo_level> result;
    m_impl->param_procs.consume(id, [&result](stereo_level const& lvl) {
        result = lvl;
    });
    return result;
}

void
audio_engine::rebuild(
        mixer::state const& mixer_state,
        fx::modules_t const& fx_modules,
        fx::parameters_t const& fx_params,
        parameter_maps const& params,
        fx::ladspa_processor_factory const& ladspa_fx_proc_factory)
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
            mixer_state.inputs,
            mixer_state.outputs,
            m_impl->param_procs,
            ladspa_fx_proc_factory);

    m_impl->param_procs.initialize(params);

    auto input_solo_index_proc =
            std::make_unique<ns_ae::value_input_processor<mixer::bus_id>>(
                    mixer_state.input_solo_id,
                    "input_solo_index");
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

    audio::engine::bypass_event_identity_processors(new_graph);

    m_impl->process.swap_executor(
            ns_ae::graph_to_dag(new_graph).make_runnable(m_wt_configs));

    m_impl->graph = std::move(new_graph);
    m_impl->input_buses = std::move(input_buses);
    m_impl->output_buses = std::move(output_buses);
    m_impl->fx_chains = std::move(fx_chains);
    m_impl->input_solo_index_proc = std::move(input_solo_index_proc);
    m_impl->mixer_procs = std::move(mixers);

    m_impl->param_procs.clear_expired();

    std::ofstream("graph.dot")
            << audio::engine::export_graph_as_dot(m_impl->graph) << std::endl;
}

void
audio_engine::operator()(
        range::table_view<float const> const& in_audio,
        range::table_view<float> const& out_audio) noexcept
{
    m_impl->process(in_audio, out_audio);
}

} // namespace piejam::runtime
