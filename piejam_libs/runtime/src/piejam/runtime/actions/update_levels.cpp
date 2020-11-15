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

#include <piejam/runtime/actions/update_levels.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <>
auto
reduce_mixer_state(
        audio_state const& st,
        update_levels const& a) -> mixer::state
{
    auto mixer_state = st.mixer_state;

    auto const update =
            [](auto& channels_map, auto const& channels, auto const& levels) {
                std::size_t const num_levels = levels.size();
                BOOST_ASSERT(num_levels == channels.size());
                for (std::size_t index = 0; index < num_levels; ++index)
                    channels_map[channels[index]].level = levels[index];
            };

    update(mixer_state.buses, mixer_state.inputs, a.in_levels);
    update(mixer_state.buses, mixer_state.outputs, a.out_levels);

    return mixer_state;
}

template auto reduce(audio_state const&, update_levels const&)
        -> audio_state;

} // namespace piejam::runtime::actions
