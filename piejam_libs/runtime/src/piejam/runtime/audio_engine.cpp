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
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/mixer.h>
#include <piejam/thread/configuration.h>

#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <optional>
#include <ranges>

namespace piejam::runtime
{

namespace ns_ae = audio::engine;

using processor_ptr = std::unique_ptr<ns_ae::processor>;

class audio_engine::mixer_bus final
{
public:
    mixer_bus(
            audio::samplerate_t const samplerate,
            mixer::bus_id bus_id,
            mixer::bus const& channel)
        : m_id(bus_id)
        , m_volume_input_proc(
                  std::make_unique<ns_ae::value_input_processor<float>>(
                          "volume"))
        , m_pan_balance_input_proc(
                  std::make_unique<ns_ae::value_input_processor<float>>(
                          channel.type == audio::bus_type::mono ? "pan"
                                                                : "balance"))
        , m_mute_input_proc(
                  std::make_unique<ns_ae::value_input_processor<bool>>("mute"))
        , m_pan_balance(
                  channel.type == audio::bus_type::mono
                          ? audio::components::make_pan()
                          : audio::components::make_stereo_balance())
        , m_volume_amp(audio::components::make_stereo_amplifier("volume"))
        , m_mute_solo(components::make_mute_solo(bus_id))
        , m_level_meter(audio::components::make_stereo_level_meter(samplerate))
        , m_peak_level_proc(
                  std::make_unique<
                          audio::engine::value_output_processor<stereo_level>>(
                          "stereo_level"))
    {
    }

    auto id() const noexcept -> mixer::bus_id { return m_id; }

    void set_volume(float vol) noexcept { m_volume_input_proc->set(vol); }

    void set_pan_balance(float pan) noexcept
    {
        m_pan_balance_input_proc->set(pan);
    }

    void set_mute(bool const mute) noexcept { m_mute_input_proc->set(mute); }

    auto get_level() const noexcept -> stereo_level const&
    {
        m_peak_level_proc->consume(
                [this](auto const& lvl) { m_last_level = lvl; });
        return m_last_level;
    }

    auto volume_input_processor() const noexcept
            -> ns_ae::value_input_processor<float>&
    {
        return *m_volume_input_proc;
    }

    auto pan_balance_input_processor() const noexcept
            -> ns_ae::value_input_processor<float>&
    {
        return *m_pan_balance_input_proc;
    }

    auto mute_input_processor() const noexcept
            -> ns_ae::value_input_processor<bool>&
    {
        return *m_mute_input_proc;
    }

    auto pan_balance_component() const noexcept -> ns_ae::component&
    {
        return *m_pan_balance;
    }

    auto volume_amp_component() const noexcept -> ns_ae::component&
    {
        return *m_volume_amp;
    }

    auto mute_solo_component() const noexcept -> ns_ae::component&
    {
        return *m_mute_solo;
    }

    auto level_meter_component() const noexcept -> ns_ae::component&
    {
        return *m_level_meter;
    }

    auto peak_level_processor() const noexcept -> ns_ae::processor&
    {
        return *m_peak_level_proc;
    }

private:
    mixer::bus_id m_id;
    std::unique_ptr<ns_ae::value_input_processor<float>> m_volume_input_proc;
    std::unique_ptr<ns_ae::value_input_processor<float>>
            m_pan_balance_input_proc;
    std::unique_ptr<ns_ae::value_input_processor<bool>> m_mute_input_proc;
    component_ptr m_pan_balance;
    component_ptr m_volume_amp;
    component_ptr m_mute_solo;
    component_ptr m_level_meter;
    std::unique_ptr<ns_ae::value_output_processor<stereo_level>>
            m_peak_level_proc;
    mutable stereo_level m_last_level{};
};

static auto
make_mixer_bus_vector(
        std::vector<audio_engine::mixer_bus>& prev_buses,
        unsigned const samplerate,
        mixer::buses_t const& chs,
        mixer::bus_list_t const& ch_ids) -> std::vector<audio_engine::mixer_bus>
{
    std::vector<audio_engine::mixer_bus> result;
    result.reserve(ch_ids.size());

    for (auto const& id : ch_ids)
    {
        if (auto it = std::ranges::find(
                    prev_buses,
                    id,
                    &audio_engine::mixer_bus::id);
            it != prev_buses.end())
        {
            result.emplace_back(std::move(*it));
        }
        else
        {
            result.emplace_back(samplerate, id, chs[id]);
        }
    }

    return result;
}

using float_input_parameter_procs = std::
        map<float_parameter_id, audio::engine::value_input_processor<float>*>;

static auto
gather_float_input_parameters(
        mixer::state const& mixer_state,
        std::vector<audio_engine::mixer_bus> const& buses,
        float_input_parameter_procs& procs)
{
    for (auto const& mb : buses)
    {
        auto const& bus = mixer_state.buses[mb.id()];
        procs.emplace(bus.volume, &mb.volume_input_processor());
        procs.emplace(bus.pan_balance, &mb.pan_balance_input_processor());
    }
}

static auto
initialize_float_input_parameters(
        float_parameters const& floats,
        float_input_parameter_procs const& procs)
{
    for (auto&& [id, proc] : procs)
    {
        proc->set(*floats.get(id));
    }
}

using bool_input_parameter_procs = std::
        map<bool_parameter_id, audio::engine::value_input_processor<bool>*>;

static auto
gather_bool_input_parameters(
        mixer::state const& mixer_state,
        std::vector<audio_engine::mixer_bus> const& buses,
        bool_input_parameter_procs& procs)
{
    for (auto const& mb : buses)
    {
        auto const& bus = mixer_state.buses[mb.id()];
        procs.emplace(bus.mute, &mb.mute_input_processor());
    }
}

static auto
initialize_bool_input_parameters(
        bool_parameters const& bools,
        bool_input_parameter_procs const& procs)
{
    for (auto&& [id, proc] : procs)
    {
        proc->set(*bools.get(id));
    }
}

static void
connect_mixer_bus(ns_ae::graph& g, audio_engine::mixer_bus const& mb)
{
    mb.pan_balance_component().connect(g);
    mb.volume_amp_component().connect(g);
    mb.mute_solo_component().connect(g);
    mb.level_meter_component().connect(g);

    g.add_event_wire(
            {mb.pan_balance_input_processor(), 0},
            mb.pan_balance_component().event_inputs()[0]);
    g.add_event_wire(
            {mb.volume_input_processor(), 0},
            mb.volume_amp_component().event_inputs()[0]);
    g.add_event_wire(
            {mb.mute_input_processor(), 0},
            mb.mute_solo_component().event_inputs()[0]);

    audio::engine::connect_stereo_components(
            g,
            mb.pan_balance_component(),
            mb.volume_amp_component());
    audio::engine::connect_stereo_components(
            g,
            mb.volume_amp_component(),
            mb.mute_solo_component());
    audio::engine::connect_stereo_components(
            g,
            mb.volume_amp_component(),
            mb.level_meter_component());

    g.add_event_wire(
            mb.level_meter_component().event_outputs()[0],
            {mb.peak_level_processor(), 0});
}

static auto
make_graph(
        mixer::state const& mixer_state,
        ns_ae::processor& input_proc,
        ns_ae::processor& output_proc,
        std::vector<audio_engine::mixer_bus> const& input_buses,
        std::vector<audio_engine::mixer_bus> const& output_buses,
        ns_ae::processor& input_solo_index,
        std::vector<processor_ptr>& mixer_procs)
{
    ns_ae::graph g;

    BOOST_ASSERT(mixer_state.inputs->size() == input_buses.size());
    for (std::size_t idx = 0; idx < mixer_state.inputs->size(); ++idx)
    {
        auto const& bus =
                mixer::get_bus<audio::bus_direction::input>(mixer_state, idx);
        auto const& mb = input_buses[idx];

        connect_mixer_bus(g, mb);

        if (bus.device_channels.left != npos)
            g.add_wire(
                    {input_proc, bus.device_channels.left},
                    mb.pan_balance_component().inputs()[0]);

        if (bus.device_channels.right != npos)
            g.add_wire(
                    {input_proc, bus.device_channels.right},
                    mb.pan_balance_component().inputs()[1]);

        g.add_event_wire(
                {input_solo_index, 0},
                mb.mute_solo_component().event_inputs()[1]);
    }

    BOOST_ASSERT(mixer_state.outputs->size() == output_buses.size());
    for (std::size_t idx = 0; idx < mixer_state.outputs->size(); ++idx)
    {
        auto const& bus =
                mixer::get_bus<audio::bus_direction::output>(mixer_state, idx);
        auto const& mb = output_buses[idx];

        connect_mixer_bus(g, mb);

        if (bus.device_channels.left != npos)
            connect(g,
                    mb.mute_solo_component().outputs()[0],
                    {output_proc, bus.device_channels.left},
                    mixer_procs);

        if (bus.device_channels.right != npos)
            connect(g,
                    mb.mute_solo_component().outputs()[1],
                    {output_proc, bus.device_channels.right},
                    mixer_procs);
    }

    for (auto const& out : output_buses)
    {
        for (auto const& in : input_buses)
        {
            connect_stereo_components(
                    g,
                    in.mute_solo_component(),
                    out.pan_balance_component(),
                    mixer_procs);
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
    , m_process(std::make_unique<audio::engine::process>(
              num_device_input_channels,
              num_device_output_channels))
{
}

audio_engine::~audio_engine() = default;

void
audio_engine::set_bool_parameter(bool_parameter_id const id, bool const value)
{
    auto it = m_bool_input_parameter_procs.find(id);
    if (it != m_bool_input_parameter_procs.end())
    {
        it->second->set(value);
    }
}

void
audio_engine::set_float_parameter(
        float_parameter_id const id,
        float const value)
{
    auto it = m_float_input_parameter_procs.find(id);
    if (it != m_float_input_parameter_procs.end())
    {
        it->second->set(value);
    }
}

void
audio_engine::set_input_channel_pan_balance(
        std::size_t const index,
        float const pan_balance)
{
    m_input_buses[index].set_pan_balance(pan_balance);
}

void
audio_engine::set_input_channel_mute(std::size_t const index, bool const mute)
{
    m_input_buses[index].set_mute(mute);
}

void
audio_engine::set_input_solo(mixer::bus_id const& id)
{
    m_input_solo_index_proc->set(id);
}

void
audio_engine::set_output_channel_balance(
        std::size_t const index,
        float const balance)
{
    m_output_buses[index].set_pan_balance(balance);
}

void
audio_engine::set_output_channel_mute(std::size_t const index, bool const mute)
{
    m_output_buses[index].set_mute(mute);
}

auto
audio_engine::get_input_level(std::size_t const index) const noexcept
        -> stereo_level
{
    return m_input_buses[index].get_level();
}

auto
audio_engine::get_output_level(std::size_t const index) const noexcept
        -> stereo_level
{
    return m_output_buses[index].get_level();
}

void
audio_engine::rebuild(
        mixer::state const& mixer_state,
        bool_parameters const& bool_params,
        float_parameters const& float_params)
{
    float_input_parameter_procs float_input_parameter_procs;
    bool_input_parameter_procs bool_input_parameter_procs;

    auto input_buses = make_mixer_bus_vector(
            m_input_buses,
            m_samplerate,
            mixer_state.buses,
            mixer_state.inputs);
    gather_float_input_parameters(
            mixer_state,
            input_buses,
            float_input_parameter_procs);
    gather_bool_input_parameters(
            mixer_state,
            input_buses,
            bool_input_parameter_procs);
    auto output_buses = make_mixer_bus_vector(
            m_output_buses,
            m_samplerate,
            mixer_state.buses,
            mixer_state.outputs);
    gather_float_input_parameters(
            mixer_state,
            output_buses,
            float_input_parameter_procs);
    gather_bool_input_parameters(
            mixer_state,
            output_buses,
            bool_input_parameter_procs);

    initialize_float_input_parameters(
            float_params,
            float_input_parameter_procs);
    initialize_bool_input_parameters(bool_params, bool_input_parameter_procs);

    auto input_solo_index_proc =
            std::make_unique<ns_ae::value_input_processor<mixer::bus_id>>(
                    mixer_state.input_solo_id,
                    "input_solo_index");
    std::vector<processor_ptr> mixers;

    auto new_graph = make_graph(
            mixer_state,
            m_process->input(),
            m_process->output(),
            input_buses,
            output_buses,
            *input_solo_index_proc,
            mixers);

    m_process->swap_executor(
            ns_ae::graph_to_dag(new_graph).make_runnable(m_wt_configs));

    m_graph = std::move(new_graph);
    m_input_buses = std::move(input_buses);
    m_output_buses = std::move(output_buses);
    m_input_solo_index_proc = std::move(input_solo_index_proc);
    m_mixer_procs = std::move(mixers);
    m_float_input_parameter_procs = std::move(float_input_parameter_procs);
    m_bool_input_parameter_procs = std::move(bool_input_parameter_procs);

    std::ofstream("graph.dot")
            << audio::engine::export_graph_as_dot(m_graph) << std::endl;
}

void
audio_engine::operator()(
        range::table_view<float const> const& in_audio,
        range::table_view<float> const& out_audio) noexcept
{
    (*m_process)(in_audio, out_audio);
}

} // namespace piejam::runtime
