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

#include <piejam/audio/level_meter.h>
#include <piejam/audio/mixer.h>
#include <piejam/audio/pair.h>
#include <piejam/audio/smoother.h>

#include <piejam/range/fwd.h>

#include <atomic>
#include <vector>

namespace piejam::audio
{

class engine
{
public:
    engine(unsigned samplerate, std::size_t num_inputs);

    void set_input_channel_enabled(std::size_t index, bool enabled);
    void set_input_channel_gain(std::size_t index, float gain);
    void set_input_channel_pan(std::size_t index, float pan);
    void set_output_channel_gain(float gain);
    void set_output_channel_balance(float balance);

    auto get_input_level(std::size_t index) const noexcept
            -> mixer::stereo_level;
    auto get_output_level() const noexcept -> mixer::stereo_level;

    void operator()(
            range::table_view<float const> const& in_audio,
            range::table_view<float> const& out_audio) noexcept;

private:
    struct mixer_channel
    {
        std::atomic_bool enabled{true};
        std::atomic<float> gain{1.f};
        pair<std::atomic<float>> level;
    };

    struct input_mixer_channel : mixer_channel
    {
        std::atomic<float> pan{};
    };

    struct output_mixer_channel : mixer_channel
    {
        std::atomic<float> balance;
    };

    struct mixer_state
    {
        mixer_state(std::size_t num_inputs)
            : inputs(num_inputs)
        {
        }

        std::vector<input_mixer_channel> inputs;
        output_mixer_channel output;
    };

    mixer_state m_mixer_state;

    std::vector<pair<level_meter>> m_in_level_meters;
    pair<level_meter> m_out_level_meter;

    std::vector<pair<smoother<>>> m_in_gain_smoothers;
    pair<smoother<>> m_out_gain_smoother;
};

} // namespace piejam::audio
