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
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/event_to_audio_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_algorithms.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/input_processor.h>
#include <piejam/audio/engine/level_meter_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/multiply_processor.h>
#include <piejam/audio/engine/output_processor.h>
#include <piejam/audio/engine/process.h>
#include <piejam/audio/engine/select_processor.h>
#include <piejam/audio/engine/value_input_processor.h>
#include <piejam/audio/engine/value_output_processor.h>
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/processors/mixer_bus_processor.h>
#include <piejam/runtime/processors/mute_solo_processor.h>
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
            mixer::bus_id ch_id,
            mixer::bus const& channel)
        : m_device_channels(channel.device_channels)
        , m_volume_proc(std::make_unique<ns_ae::value_input_processor<float>>(
                  channel.volume,
                  "volume"))
        , m_pan_balance_proc(
                  std::make_unique<ns_ae::value_input_processor<float>>(
                          channel.pan_balance,
                          channel.type == audio::bus_type::mono ? "pan"
                                                                : "balance"))
        , m_mute_input_proc(
                  std::make_unique<ns_ae::value_input_processor<bool>>(
                          channel.mute,
                          "mute"))
        , m_channel_proc(
                  channel.type == audio::bus_type::mono
                          ? processors::make_mono_mixer_bus_processor()
                          : processors::make_stereo_mixer_bus_processor())
        , m_amp(audio::components::make_stereo_split_amplifier("volume"))
        , m_mute_solo_proc(processors::make_mute_solo_processor(ch_id))
        , m_mute_amp(audio::components::make_stereo_amplifier("mute"))
        , m_level_meter(audio::components::make_stereo_level_meter(samplerate))
        , m_peak_level_proc(
                  std::make_unique<audio::engine::value_output_processor<
                          mixer::stereo_level>>("stereo_level"))
    {
    }

    auto device_channels() const noexcept -> channel_index_pair const&
    {
        return m_device_channels;
    }

    void set_volume(float vol) noexcept { m_volume_proc->set(vol); }
    void set_pan_balance(float pan) noexcept { m_pan_balance_proc->set(pan); }
    void set_mute(bool const mute) noexcept { m_mute_input_proc->set(mute); }

    auto get_level() const noexcept -> mixer::stereo_level const&
    {
        m_peak_level_proc->consume(
                [this](auto const& lvl) { m_last_level = lvl; });
        return m_last_level;
    }

    auto volume_processor() const noexcept -> ns_ae::processor&
    {
        return *m_volume_proc;
    }

    auto pan_balance_processor() const noexcept -> ns_ae::processor&
    {
        return *m_pan_balance_proc;
    }

    auto mute_input_processor() const noexcept -> ns_ae::processor&
    {
        return *m_mute_input_proc;
    }

    auto channel_processor() const noexcept -> ns_ae::processor&
    {
        return *m_channel_proc;
    }

    auto amplifier_component() const noexcept -> ns_ae::component&
    {
        return *m_amp;
    }

    auto mute_solo_processor() const noexcept -> ns_ae::processor&
    {
        return *m_mute_solo_proc;
    }

    auto mute_amplifier_component() const noexcept -> ns_ae::component&
    {
        return *m_mute_amp;
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
    channel_index_pair m_device_channels;
    std::unique_ptr<ns_ae::value_input_processor<float>> m_volume_proc;
    std::unique_ptr<ns_ae::value_input_processor<float>> m_pan_balance_proc;
    std::unique_ptr<ns_ae::value_input_processor<bool>> m_mute_input_proc;
    processor_ptr m_channel_proc;
    component_ptr m_amp;
    processor_ptr m_mute_solo_proc;
    component_ptr m_mute_amp;
    component_ptr m_level_meter;
    std::unique_ptr<ns_ae::value_output_processor<mixer::stereo_level>>
            m_peak_level_proc;
    mutable mixer::stereo_level m_last_level{};
};

static auto
make_mixer_bus_vector(
        unsigned const samplerate,
        mixer::buses_t const& chs,
        mixer::bus_ids_t const& ch_ids) -> std::vector<audio_engine::mixer_bus>
{
    std::vector<audio_engine::mixer_bus> result;
    result.reserve(ch_ids.size());
    for (auto const& id : ch_ids)
        result.emplace_back(samplerate, id, chs[id]);
    return result;
}

static void
connect_mixer_bus(ns_ae::graph& g, audio_engine::mixer_bus const& mb)
{
    g.add_event_wire(
            {mb.pan_balance_processor(), 0},
            {mb.channel_processor(), 0});
    g.add_event_wire({mb.volume_processor(), 0}, {mb.channel_processor(), 1});
    g.add_event_wire(
            {mb.channel_processor(), 0},
            mb.amplifier_component().event_inputs()[0]);
    g.add_event_wire(
            {mb.channel_processor(), 1},
            mb.amplifier_component().event_inputs()[1]);
    g.add_event_wire(
            {mb.mute_input_processor(), 0},
            {mb.mute_solo_processor(), 0});
    g.add_event_wire(
            {mb.mute_solo_processor(), 0},
            mb.mute_amplifier_component().event_inputs()[0]);
    mb.amplifier_component().connect(g);
    mb.level_meter_component().connect(g);
    audio::engine::connect_stereo_components(
            g,
            mb.amplifier_component(),
            mb.level_meter_component());
    g.add_event_wire(
            mb.level_meter_component().event_outputs()[0],
            {mb.peak_level_processor(), 0});
    audio::engine::connect_stereo_components(
            g,
            mb.amplifier_component(),
            mb.mute_amplifier_component());
    mb.mute_amplifier_component().connect(g);
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

    for (auto& mb : input_buses)
    {
        connect_mixer_bus(g, mb);

        if (mb.device_channels().left != npos)
            g.add_wire(
                    {input_proc, mb.device_channels().left},
                    mb.amplifier_component().inputs()[0]);

        if (mb.device_channels().right != npos)
            g.add_wire(
                    {input_proc, mb.device_channels().right},
                    mb.amplifier_component().inputs()[1]);

        g.add_event_wire({input_solo_index, 0}, {mb.mute_solo_processor(), 1});
    }

    for (auto& mb : output_buses)
    {
        connect_mixer_bus(g, mb);

        if (mb.device_channels().left != npos)
            connect(g,
                    mb.mute_amplifier_component().outputs()[0],
                    {output_proc, mb.device_channels().left},
                    mixer_procs);

        if (mb.device_channels().right != npos)
            connect(g,
                    mb.mute_amplifier_component().outputs()[1],
                    {output_proc, mb.device_channels().right},
                    mixer_procs);
    }

    for (std::size_t out = 0, num_outputs = mixer_state.outputs.size();
         out < num_outputs;
         ++out)
    {
        for (std::size_t in = 0, num_inputs = mixer_state.inputs.size();
             in < num_inputs;
             ++in)
        {
            connect(g,
                    input_buses[in].mute_amplifier_component().outputs()[0],
                    output_buses[out].amplifier_component().inputs()[0],
                    mixer_procs);

            connect(g,
                    input_buses[in].mute_amplifier_component().outputs()[1],
                    output_buses[out].amplifier_component().inputs()[1],
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
audio_engine::set_input_channel_volume(
        std::size_t const index,
        float const volume)
{
    m_input_buses[index].set_volume(volume);
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
audio_engine::set_output_channel_volume(
        std::size_t const index,
        float const volume)
{
    m_output_buses[index].set_volume(volume);
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
        -> mixer::stereo_level
{
    return m_input_buses[index].get_level();
}

auto
audio_engine::get_output_level(std::size_t const index) const noexcept
        -> mixer::stereo_level
{
    return m_output_buses[index].get_level();
}

void
audio_engine::rebuild(mixer::state const& mixer_state)
{
    auto input_buses = make_mixer_bus_vector(
            m_samplerate,
            mixer_state.buses,
            mixer_state.inputs);
    auto output_buses = make_mixer_bus_vector(
            m_samplerate,
            mixer_state.buses,
            mixer_state.outputs);
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
