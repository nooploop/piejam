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

#include <piejam/audio/components/level_meter_processor.h>
#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/event_to_audio_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_algorithms.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/input_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/multiply_processor.h>
#include <piejam/audio/engine/output_processor.h>
#include <piejam/audio/engine/select_processor.h>
#include <piejam/audio/engine/value_input_processor.h>
#include <piejam/runtime/audio_components/mixer_bus_processor.h>
#include <piejam/runtime/audio_components/mute_solo_processor.h>
#include <piejam/runtime/channel_index_pair.h>

#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <ranges>

namespace piejam::runtime
{

namespace aucomp = audio::components;
namespace ns_ae = audio::engine;

using processor_ptr = std::unique_ptr<ns_ae::processor>;

class audio_engine::mixer_bus final
{
public:
    mixer_bus(
            unsigned const samplerate,
            std::size_t channel_index,
            mixer::channel const& channel)
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
                          ? audio_components::make_mono_mixer_bus_processor()
                          : audio_components::make_stereo_mixer_bus_processor())
        , m_amp_smoother_procs(
                  ns_ae::make_event_to_audio_processor("L"),
                  ns_ae::make_event_to_audio_processor("R"))
        , m_amp_multiply_procs(
                  ns_ae::make_multiply_processor(2, "L"),
                  ns_ae::make_multiply_processor(2, "R"))
        , m_mute_solo_proc(
                  audio_components::make_mute_solo_processor(channel_index))
        , m_mute_smoother_procs(
                  ns_ae::make_event_to_audio_processor("mute L"),
                  ns_ae::make_event_to_audio_processor("mute R"))
        , m_mute_multiply_procs(
                  ns_ae::make_multiply_processor(2, "mute amp L"),
                  ns_ae::make_multiply_processor(2, "mute amp R"))
        , m_level_meter_procs(
                  std::make_unique<aucomp::level_meter_processor>(
                          samplerate,
                          "L"),
                  std::make_unique<aucomp::level_meter_processor>(
                          samplerate,
                          "R"))
    {
    }

    auto device_channels() const noexcept -> channel_index_pair const&
    {
        return m_device_channels;
    }

    void set_volume(float vol) noexcept { m_volume_proc->set(vol); }
    void set_pan_balance(float pan) noexcept { m_pan_balance_proc->set(pan); }
    void set_mute(bool const mute) noexcept { m_mute_input_proc->set(mute); }

    auto get_level() const noexcept -> mixer::stereo_level
    {
        return {m_level_meter_procs.left->peak_level(),
                m_level_meter_procs.right->peak_level()};
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

    auto left_amp_smoother_processor() const noexcept -> ns_ae::processor&
    {
        return *m_amp_smoother_procs.left;
    }

    auto right_amp_smoother_processor() const noexcept -> ns_ae::processor&
    {
        return *m_amp_smoother_procs.right;
    }

    auto left_amp_multiply_processor() const noexcept -> ns_ae::processor&
    {
        return *m_amp_multiply_procs.left;
    }

    auto right_amp_multiply_processor() const noexcept -> ns_ae::processor&
    {
        return *m_amp_multiply_procs.right;
    }

    auto mute_solo_processor() const noexcept -> ns_ae::processor&
    {
        return *m_mute_solo_proc;
    }

    auto left_mute_smoother_processor() const noexcept -> ns_ae::processor&
    {
        return *m_mute_smoother_procs.left;
    }

    auto right_mute_smoother_processor() const noexcept -> ns_ae::processor&
    {
        return *m_mute_smoother_procs.right;
    }

    auto left_mute_multiply_processor() const noexcept -> ns_ae::processor&
    {
        return *m_mute_multiply_procs.left;
    }

    auto right_mute_multiply_processor() const noexcept -> ns_ae::processor&
    {
        return *m_mute_multiply_procs.right;
    }

    auto left_level_meter_processor() const noexcept -> ns_ae::processor&
    {
        return *m_level_meter_procs.left;
    }

    auto right_level_meter_processor() const noexcept -> ns_ae::processor&
    {
        return *m_level_meter_procs.right;
    }

private:
    channel_index_pair m_device_channels;
    std::unique_ptr<ns_ae::value_input_processor<float>> m_volume_proc;
    std::unique_ptr<ns_ae::value_input_processor<float>> m_pan_balance_proc;
    std::unique_ptr<ns_ae::value_input_processor<bool>> m_mute_input_proc;
    processor_ptr m_channel_proc;
    audio::pair<processor_ptr> m_amp_smoother_procs;
    audio::pair<processor_ptr> m_amp_multiply_procs;
    processor_ptr m_mute_solo_proc;
    audio::pair<processor_ptr> m_mute_smoother_procs;
    audio::pair<processor_ptr> m_mute_multiply_procs;
    audio::pair<std::unique_ptr<aucomp::level_meter_processor>>
            m_level_meter_procs;
};

static auto
make_mixer_bus_vector(
        unsigned const samplerate,
        mixer::channels const& channels) -> std::vector<audio_engine::mixer_bus>
{
    std::vector<audio_engine::mixer_bus> result;
    result.reserve(channels.size());
    for (std::size_t ch = 0, chs = channels.size(); ch < chs; ++ch)
        result.emplace_back(samplerate, ch, channels[ch]);
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
            {mb.left_amp_smoother_processor(), 0});
    g.add_event_wire(
            {mb.channel_processor(), 1},
            {mb.right_amp_smoother_processor(), 0});
    g.add_event_wire(
            {mb.mute_input_processor(), 0},
            {mb.mute_solo_processor(), 0});
    g.add_event_wire(
            {mb.mute_solo_processor(), 0},
            {mb.left_mute_smoother_processor(), 0});
    g.add_event_wire(
            {mb.mute_solo_processor(), 0},
            {mb.right_mute_smoother_processor(), 0});
    g.add_wire(
            {mb.left_amp_smoother_processor(), 0},
            {mb.left_amp_multiply_processor(), 1});
    g.add_wire(
            {mb.right_amp_smoother_processor(), 0},
            {mb.right_amp_multiply_processor(), 1});
    g.add_wire(
            {mb.left_amp_multiply_processor(), 0},
            {mb.left_level_meter_processor(), 0});
    g.add_wire(
            {mb.right_amp_multiply_processor(), 0},
            {mb.right_level_meter_processor(), 0});
    g.add_wire(
            {mb.left_amp_multiply_processor(), 0},
            {mb.left_mute_multiply_processor(), 0});
    g.add_wire(
            {mb.right_amp_multiply_processor(), 0},
            {mb.right_mute_multiply_processor(), 0});
    g.add_wire(
            {mb.left_mute_smoother_processor(), 0},
            {mb.left_mute_multiply_processor(), 1});
    g.add_wire(
            {mb.right_mute_smoother_processor(), 0},
            {mb.right_mute_multiply_processor(), 1});
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
                    {mb.left_amp_multiply_processor(), 0});

        if (mb.device_channels().right != npos)
            g.add_wire(
                    {input_proc, mb.device_channels().right},
                    {mb.right_amp_multiply_processor(), 0});

        g.add_event_wire({input_solo_index, 0}, {mb.mute_solo_processor(), 1});
    }

    for (auto& mb : output_buses)
    {
        connect_mixer_bus(g, mb);

        if (mb.device_channels().left != npos)
            connect(g,
                    {mb.left_mute_multiply_processor(), 0},
                    {output_proc, mb.device_channels().left},
                    mixer_procs);

        if (mb.device_channels().right != npos)
            connect(g,
                    {mb.right_mute_multiply_processor(), 0},
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
                    {input_buses[in].left_mute_multiply_processor(), 0},
                    {output_buses[out].left_amp_multiply_processor(), 0},
                    mixer_procs);

            connect(g,
                    {input_buses[in].right_mute_multiply_processor(), 0},
                    {output_buses[out].right_amp_multiply_processor(), 0},
                    mixer_procs);
        }
    }

    return g;
}

audio_engine::audio_engine(
        std::span<thread::configuration const> const& wt_configs,
        unsigned const samplerate,
        unsigned const num_device_input_channels,
        unsigned const num_device_output_channels,
        mixer::state const& mixer_state)
    : m_input_proc(std::make_unique<ns_ae::input_processor>(
              num_device_input_channels))
    , m_output_proc(std::make_unique<ns_ae::output_processor>(
              num_device_output_channels))
    , m_input_buses(make_mixer_bus_vector(samplerate, mixer_state.inputs))
    , m_output_buses(make_mixer_bus_vector(samplerate, mixer_state.outputs))
    , m_input_solo_index_proc(
              std::make_unique<ns_ae::value_input_processor<std::size_t>>(
                      mixer_state.input_solo_index,
                      "input_solo_index"))
    , m_graph(make_graph(
              mixer_state,
              *m_input_proc,
              *m_output_proc,
              m_input_buses,
              m_output_buses,
              *m_input_solo_index_proc,
              m_mixer_procs))
    , m_dag(ns_ae::graph_to_dag(m_graph, m_buffer_size)
                    .make_runnable(wt_configs))
{
    std::ofstream("graph.dot")
            << audio::engine::export_graph_as_dot(m_graph) << std::endl;
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
audio_engine::set_input_solo(std::size_t const index)
{
    BOOST_ASSERT(index == npos || index < m_input_buses.size());
    m_input_solo_index_proc->set(index);
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
audio_engine::operator()(
        range::table_view<float const> const& in_audio,
        range::table_view<float> const& out_audio) noexcept
{
    m_input_proc->set_input(in_audio);
    m_output_proc->set_output(out_audio);
    m_buffer_size = in_audio.minor_size();

    (*m_dag)();
}

} // namespace piejam::runtime
