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

#include <piejam/runtime/actions/select_bus_channel.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <>
auto
reduce_mixer_state(audio_state const& st, select_bus_channel const& a)
        -> mixer::state
{
    auto mixer_state = st.mixer_state;

    auto& channels = a.direction == audio::bus_direction::input
                             ? mixer_state.inputs
                             : mixer_state.outputs;

    BOOST_ASSERT(a.bus < channels.size());
    BOOST_ASSERT(
            a.channel_index == npos ||
            a.channel_index < (a.direction == audio::bus_direction::input
                                       ? st.input.hw_params->num_channels
                                       : st.output.hw_params->num_channels));

    auto& ch = mixer_state.buses[channels[a.bus]];
    switch (a.channel_selector)
    {
        case audio::bus_channel::mono:
            BOOST_ASSERT(a.direction == audio::bus_direction::input);
            ch.device_channels = channel_index_pair{a.channel_index};
            break;

        case audio::bus_channel::left:
            BOOST_ASSERT(
                    a.direction == audio::bus_direction::input ||
                    a.direction == audio::bus_direction::output);
            ch.device_channels.left = a.channel_index;
            break;

        case audio::bus_channel::right:
            BOOST_ASSERT(
                    a.direction == audio::bus_direction::input ||
                    a.direction == audio::bus_direction::output);
            ch.device_channels.right = a.channel_index;
            break;
    }

    return mixer_state;
}

template auto reduce(audio_state const&, select_bus_channel const&)
        -> audio_state;

} // namespace piejam::runtime::actions
