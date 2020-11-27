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
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/components/mixer_bus.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/thread/configuration.h>

#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <optional>
#include <ranges>

namespace piejam::runtime
{

namespace ns_ae = audio::engine;

using processor_ptr = std::unique_ptr<audio::engine::processor>;
using component_ptr = std::unique_ptr<audio::engine::component>;

struct audio_engine::impl
{
    impl(unsigned num_device_input_channels,
         unsigned num_device_output_channels)
        : process(num_device_input_channels, num_device_output_channels)
    {
    }

    audio::engine::process process;

    std::vector<std::pair<mixer::bus_id, component_ptr>> input_buses;
    std::vector<std::pair<mixer::bus_id, component_ptr>> output_buses;
    std::unique_ptr<audio::engine::value_input_processor<mixer::bus_id>>
            input_solo_index_proc;
    std::vector<processor_ptr> mixer_procs;

    parameter_processor_factory param_procs;

    audio::engine::graph graph;
};

static auto
make_mixer_bus_vector(
        std::vector<std::pair<mixer::bus_id, component_ptr>>& prev_buses,
        unsigned const samplerate,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& bus_ids,
        parameter_processor_factory& param_procs)
        -> std::vector<std::pair<mixer::bus_id, component_ptr>>
{
    std::vector<std::pair<mixer::bus_id, component_ptr>> result;
    result.reserve(bus_ids.size());

    for (auto const& id : bus_ids)
    {
        if (auto it = std::ranges::find(
                    prev_buses,
                    id,
                    &std::pair<mixer::bus_id, component_ptr>::first);
            it != prev_buses.end())
        {
            result.emplace_back(std::move(*it));
            prev_buses.erase(it);
        }
        else
        {
            result.emplace_back(
                    id,
                    components::make_mixer_bus(
                            samplerate,
                            id,
                            *buses[id],
                            param_procs));
        }
    }

    return result;
}

static auto
make_graph(
        mixer::state const& mixer_state,
        ns_ae::processor& input_proc,
        ns_ae::processor& output_proc,
        std::vector<std::pair<mixer::bus_id, component_ptr>> const& input_buses,
        std::vector<std::pair<mixer::bus_id, component_ptr>> const&
                output_buses,
        ns_ae::processor& input_solo_index,
        std::vector<processor_ptr>& mixer_procs)
{
    ns_ae::graph g;

    BOOST_ASSERT(mixer_state.inputs->size() == input_buses.size());
    for (std::size_t idx = 0; idx < mixer_state.inputs->size(); ++idx)
    {
        auto const& bus =
                mixer::get_bus<audio::bus_direction::input>(mixer_state, idx);
        auto const& [id, mb] = input_buses[idx];

        mb->connect(g);

        if (bus.device_channels.left != npos)
            g.add_wire({input_proc, bus.device_channels.left}, mb->inputs()[0]);

        if (bus.device_channels.right != npos)
            g.add_wire(
                    {input_proc, bus.device_channels.right},
                    mb->inputs()[1]);

        g.add_event_wire({input_solo_index, 0}, mb->event_inputs()[0]);
    }

    BOOST_ASSERT(mixer_state.outputs->size() == output_buses.size());
    for (std::size_t idx = 0; idx < mixer_state.outputs->size(); ++idx)
    {
        auto const& bus =
                mixer::get_bus<audio::bus_direction::output>(mixer_state, idx);
        auto const& [id, mb] = output_buses[idx];

        mb->connect(g);

        if (bus.device_channels.left != npos)
            connect(g,
                    mb->outputs()[0],
                    {output_proc, bus.device_channels.left},
                    mixer_procs);

        if (bus.device_channels.right != npos)
            connect(g,
                    mb->outputs()[1],
                    {output_proc, bus.device_channels.right},
                    mixer_procs);
    }

    for (auto const& [id, out] : output_buses)
    {
        for (auto const& [id, in] : input_buses)
        {
            connect_stereo_components(g, *in, *out, mixer_procs);
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
        bool_parameters const& bool_params,
        float_parameters const& float_params)
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

    m_impl->param_procs.initialize(float_params);
    m_impl->param_procs.initialize(bool_params);

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
            *input_solo_index_proc,
            mixers);

    m_impl->process.swap_executor(
            ns_ae::graph_to_dag(new_graph).make_runnable(m_wt_configs));

    m_impl->graph = std::move(new_graph);
    m_impl->input_buses = std::move(input_buses);
    m_impl->output_buses = std::move(output_buses);
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
