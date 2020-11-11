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

#pragma once

#include <piejam/audio/components/fwd.h>
#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/pair.h>
#include <piejam/audio/types.h>
#include <piejam/range/table_view.h>
#include <piejam/runtime/mixer.h>
#include <piejam/thread/fwd.h>

#include <memory>
#include <span>
#include <vector>

namespace piejam::runtime
{

class audio_engine
{
public:
    class mixer_bus;

    audio_engine(
            std::span<thread::configuration const> const& wt_configs,
            audio::samplerate_t,
            unsigned num_device_input_channels,
            unsigned num_device_output_channels,
            mixer::state const&);
    ~audio_engine();

    void set_input_channel_volume(std::size_t index, float volume);
    void set_input_channel_pan_balance(std::size_t index, float pan_balance);
    void set_input_channel_mute(std::size_t index, bool mute);
    void set_input_solo(std::size_t index);
    void set_output_channel_volume(std::size_t index, float volume);
    void set_output_channel_balance(std::size_t index, float balance);
    void set_output_channel_mute(std::size_t index, bool mute);

    auto get_input_level(std::size_t index) const noexcept
            -> mixer::stereo_level;
    auto get_output_level(std::size_t index) const noexcept
            -> mixer::stereo_level;

    void operator()(
            range::table_view<float const> const& in_audio,
            range::table_view<float> const& out_audio) noexcept;

private:
    using processor_ptr = std::unique_ptr<audio::engine::processor>;

    std::unique_ptr<audio::engine::input_processor> m_input_proc;
    std::unique_ptr<audio::engine::output_processor> m_output_proc;

    std::vector<mixer_bus> m_input_buses;
    std::vector<mixer_bus> m_output_buses;
    std::unique_ptr<audio::engine::value_input_processor<std::size_t>>
            m_input_solo_index_proc;
    std::vector<processor_ptr> m_mixer_procs;

    std::size_t m_buffer_size{};
    audio::engine::graph m_graph;
    std::unique_ptr<audio::engine::dag_executor> m_dag;
};

} // namespace piejam::runtime
