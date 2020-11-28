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

#include <piejam/runtime/actions/select_device.h>

#include <piejam/runtime/audio_state.h>

#include <fmt/format.h>

namespace piejam::runtime::actions
{

template <>
auto
select_device<audio::bus_direction::input>::reduce(state const& st) const
        -> state
{
    auto new_st = st;

    new_st.input = device;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    clear_mixer_buses<audio::bus_direction::input>(new_st);

    std::size_t const num_channels = device.hw_params->num_channels;
    for (std::size_t index = 0; index < num_channels; ++index)
    {
        add_mixer_bus<audio::bus_direction::input>(
                new_st,
                fmt::format("In {}", index + 1),
                audio::bus_type::mono,
                channel_index_pair(index));
    }

    return new_st;
}

template <>
auto
select_device<audio::bus_direction::output>::reduce(state const& st) const
        -> state
{
    auto new_st = st;

    new_st.output = device;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    clear_mixer_buses<audio::bus_direction::output>(new_st);

    if (auto const num_channels = device.hw_params->num_channels)
    {
        add_mixer_bus<audio::bus_direction::output>(
                new_st,
                "Main",
                audio::bus_type::stereo,
                channel_index_pair(
                        num_channels > 0 ? 0 : npos,
                        num_channels > 1 ? 1 : npos));
    }

    return new_st;
}

} // namespace piejam::runtime::actions
