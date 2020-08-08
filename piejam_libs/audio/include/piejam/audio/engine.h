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

#include <piejam/algorithm/npos.h>
#include <piejam/audio/level_meter.h>
#include <piejam/audio/mixer.h>
#include <piejam/audio/pair.h>
#include <piejam/audio/smoother.h>
#include <piejam/audio/types.h>

#include <piejam/range/fwd.h>

#include <atomic>
#include <optional>
#include <vector>

namespace piejam::audio
{

class engine
{
public:
    engine(unsigned samplerate,
           std::vector<std::size_t> const& input_bus_config,
           std::vector<channel_index_pair> const& output_config);

    void set_input_channel_gain(std::size_t index, float gain);
    void set_input_channel_pan(std::size_t index, float pan);
    void set_output_channel_gain(std::size_t index, float gain);
    void set_output_channel_balance(std::size_t index, float balance);

    auto get_input_level(std::size_t index) const noexcept
            -> mixer::stereo_level;
    auto get_output_level(std::size_t index) const noexcept
            -> mixer::stereo_level;

    void operator()(
            range::table_view<float const> const& in_audio,
            range::table_view<float> const& out_audio) noexcept;

private:
    struct mixer_channel
    {
        bus_type type{};
        std::optional<pair<level_meter>> stereo_level_meter;
        pair<std::atomic<float>> level;
        std::atomic<float> gain{1.f};
        std::atomic<float> pan_balance{};
        pair<smoother<>> gain_smoother;
        channel_index_pair device_channels{algorithm::npos};
    };

    struct mixer_state
    {
        mixer_state(
                std::size_t num_inputs,
                std::size_t num_outputs,
                std::size_t level_meter_decay_time,
                std::size_t level_meter_rms_window_size)
            : inputs(num_inputs)
            , outputs(num_outputs)
        {
            for (auto& in : inputs)
            {
                in.stereo_level_meter = pair<level_meter>(level_meter(
                        level_meter_decay_time,
                        level_meter_rms_window_size));
            }

            for (auto& out : outputs)
            {
                out.stereo_level_meter = pair<level_meter>(level_meter(
                        level_meter_decay_time,
                        level_meter_rms_window_size));
            }
        }

        std::vector<mixer_channel> inputs;
        std::vector<mixer_channel> outputs;
    };

    mixer_state m_mixer_state;
};

} // namespace piejam::audio
