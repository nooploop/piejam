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

#include <piejam/audio/components/channel_strip_processor.h>
#include <piejam/audio/components/gui_input_processor.h>
#include <piejam/audio/components/level_meter_processor.h>
#include <piejam/audio/engine/amplify_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/input_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/output_processor.h>

#include <algorithm>
#include <ranges>

namespace piejam::runtime
{

namespace aucomp = audio::components;
using processor_ptr = std::unique_ptr<audio::engine::processor>;

class audio_engine::mixer_bus final
{
public:
    mixer_bus(unsigned const samplerate, audio::mixer::channel const& channel)
        : m_device_channels(channel.device_channels)
        , m_volume_proc(std::make_unique<aucomp::gui_input_processor<float>>(
                  channel.gain))
        , m_pan_balance_proc(
                  std::make_unique<aucomp::gui_input_processor<float>>(
                          channel.pan_balance))
        , m_channel_proc(
                  channel.type == audio::bus_type::mono
                          ? aucomp::make_mono_channel_strip_processor()
                          : aucomp::make_stereo_channel_strip_processor())
        , m_amplifier_procs(
                  audio::engine::make_amplify_processor(),
                  audio::engine::make_amplify_processor())
        , m_level_meter_procs(
                  std::make_unique<aucomp::level_meter_processor>(samplerate),
                  std::make_unique<aucomp::level_meter_processor>(samplerate))
    {
    }

    auto device_channels() const noexcept -> audio::channel_index_pair const&
    {
        return m_device_channels;
    }

    void set_volume(float vol) noexcept { m_volume_proc->set(vol); }
    void set_pan_balance(float pan) noexcept { m_pan_balance_proc->set(pan); }

    auto get_level() const noexcept -> audio::mixer::stereo_level
    {
        return {m_level_meter_procs.left->peak_level(),
                m_level_meter_procs.right->peak_level()};
    }

    auto volume_processor() const noexcept -> audio::engine::processor&
    {
        return *m_volume_proc;
    }

    auto pan_balance_processor() const noexcept -> audio::engine::processor&
    {
        return *m_pan_balance_proc;
    }

    auto channel_processor() const noexcept -> audio::engine::processor&
    {
        return *m_channel_proc;
    }

    auto left_amplifier_processor() const noexcept -> audio::engine::processor&
    {
        return *m_amplifier_procs.left;
    }

    auto right_amplifier_processor() const noexcept -> audio::engine::processor&
    {
        return *m_amplifier_procs.right;
    }

    auto left_level_meter_processor() const noexcept
            -> audio::engine::processor&
    {
        return *m_level_meter_procs.left;
    }

    auto right_level_meter_processor() const noexcept
            -> audio::engine::processor&
    {
        return *m_level_meter_procs.right;
    }

private:
    audio::channel_index_pair m_device_channels;
    std::unique_ptr<aucomp::gui_input_processor<float>> m_volume_proc;
    std::unique_ptr<aucomp::gui_input_processor<float>> m_pan_balance_proc;
    processor_ptr m_channel_proc;
    audio::pair<processor_ptr> m_amplifier_procs;
    audio::pair<std::unique_ptr<aucomp::level_meter_processor>>
            m_level_meter_procs;
};

static auto
make_mixer_bus_vector(
        unsigned const samplerate,
        audio::mixer::channels const& channels)
        -> std::vector<audio_engine::mixer_bus>
{
    std::vector<audio_engine::mixer_bus> result;
    result.reserve(channels.size());
    std::ranges::transform(
            channels,
            std::back_inserter(result),
            [samplerate](auto const& ch) {
                return audio_engine::mixer_bus(samplerate, ch);
            });
    return result;
}

static auto
make_mixer_processors(std::size_t const num_inputs, std::size_t num_outputs)
{
    std::vector<audio::pair<processor_ptr>> result;
    if (num_inputs > 1)
    {
        result.reserve(num_outputs);
        while (num_outputs--)
            result.emplace_back(
                    audio::engine::make_mix_processor(num_inputs),
                    audio::engine::make_mix_processor(num_inputs));
    }
    return result;
}

static void
connect_mixer_bus(audio::engine::graph& g, audio_engine::mixer_bus const& mb)
{
    g.add_event_wire(
            {mb.pan_balance_processor(), 0},
            {mb.channel_processor(), 0});
    g.add_event_wire({mb.volume_processor(), 0}, {mb.channel_processor(), 1});
    g.add_event_wire(
            {mb.channel_processor(), 0},
            {mb.left_amplifier_processor(), 0});
    g.add_event_wire(
            {mb.channel_processor(), 1},
            {mb.right_amplifier_processor(), 0});
    g.add_wire(
            {mb.left_amplifier_processor(), 0},
            {mb.left_level_meter_processor(), 0});
    g.add_wire(
            {mb.right_amplifier_processor(), 0},
            {mb.right_level_meter_processor(), 0});
}

static auto
make_graph(
        audio::mixer::state const& mixer_state,
        audio::engine::processor& input_proc,
        audio::engine::processor& output_proc,
        std::vector<audio_engine::mixer_bus> const& input_buses,
        std::vector<audio_engine::mixer_bus> const& output_buses,
        std::vector<audio::pair<processor_ptr>> const& mixer_procs)
{
    audio::engine::graph g;

    for (auto& mb : input_buses)
    {
        connect_mixer_bus(g, mb);

        if (mb.device_channels().left != npos)
            g.add_wire(
                    {input_proc, mb.device_channels().left},
                    {mb.left_amplifier_processor(), 0});

        if (mb.device_channels().right != npos)
            g.add_wire(
                    {input_proc, mb.device_channels().right},
                    {mb.right_amplifier_processor(), 0});
    }

    if (mixer_state.inputs.size() > 1)
    {
        for (std::size_t in = 0, num_inputs = mixer_state.inputs.size();
             in < num_inputs;
             ++in)
        {
            for (std::size_t out = 0, num_outputs = mixer_state.outputs.size();
                 out < num_outputs;
                 ++out)
            {
                g.add_wire(
                        {input_buses[in].left_amplifier_processor(), 0},
                        {*mixer_procs[out].left, in});
                g.add_wire(
                        {input_buses[in].right_amplifier_processor(), 0},
                        {*mixer_procs[out].right, in});
            }
        }
    }

    for (auto& mb : output_buses)
    {
        connect_mixer_bus(g, mb);

        if (mb.device_channels().left != npos)
            g.add_wire(
                    {mb.left_amplifier_processor(), 0},
                    {output_proc, mb.device_channels().left});

        if (mb.device_channels().right != npos)
            g.add_wire(
                    {mb.right_amplifier_processor(), 0},
                    {output_proc, mb.device_channels().right});
    }

    if (mixer_state.inputs.size() > 1)
    {
        for (std::size_t out = 0, num_outputs = mixer_state.outputs.size();
             out < num_outputs;
             ++out)
        {
            g.add_wire(
                    {*mixer_procs[out].left, 0},
                    {output_buses[out].left_amplifier_processor(), 0});

            g.add_wire(
                    {*mixer_procs[out].right, 0},
                    {output_buses[out].right_amplifier_processor(), 0});
        }
    }
    else if (mixer_state.inputs.size() == 1)
    {
        for (std::size_t out = 0, num_outputs = mixer_state.outputs.size();
             out < num_outputs;
             ++out)
        {
            g.add_wire(
                    {input_buses[0].left_amplifier_processor(), 0},
                    {output_buses[out].left_amplifier_processor(), 0});

            g.add_wire(
                    {input_buses[0].right_amplifier_processor(), 0},
                    {output_buses[out].right_amplifier_processor(), 0});
        }
    }

    return g;
}

audio_engine::audio_engine(
        unsigned const samplerate,
        unsigned const num_device_input_channels,
        unsigned const num_device_output_channels,
        audio::mixer::state const& mixer_state)
    : m_input_proc(std::make_unique<audio::engine::input_processor>(
              num_device_input_channels))
    , m_output_proc(std::make_unique<audio::engine::output_processor>(
              num_device_output_channels))
    , m_input_buses(make_mixer_bus_vector(samplerate, mixer_state.inputs))
    , m_output_buses(make_mixer_bus_vector(samplerate, mixer_state.outputs))
    , m_mixer_procs(make_mixer_processors(
              mixer_state.inputs.size(),
              mixer_state.outputs.size()))
    , m_graph(make_graph(
              mixer_state,
              *m_input_proc,
              *m_output_proc,
              m_input_buses,
              m_output_buses,
              m_mixer_procs))
    , m_dag(audio::engine::graph_to_dag(m_graph, 256, m_buffer_size))
{
}

audio_engine::~audio_engine() = default;

void
audio_engine::set_input_channel_volume(std::size_t index, float volume)
{
    m_input_buses[index].set_volume(volume);
}

void
audio_engine::set_input_channel_pan_balance(
        std::size_t index,
        float pan_balance)
{
    m_input_buses[index].set_pan_balance(pan_balance);
}

void
audio_engine::set_output_channel_volume(std::size_t index, float volume)
{
    m_output_buses[index].set_volume(volume);
}

void
audio_engine::set_output_channel_balance(std::size_t index, float balance)
{
    m_output_buses[index].set_pan_balance(balance);
}

auto
audio_engine::get_input_level(std::size_t index) const noexcept
        -> audio::mixer::stereo_level
{
    return m_input_buses[index].get_level();
}

auto
audio_engine::get_output_level(std::size_t index) const noexcept
        -> audio::mixer::stereo_level
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

    m_dag(m_thread_context);

    m_event_memory.release();
}

} // namespace piejam::runtime
