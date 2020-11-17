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

#include <piejam/runtime/actions/set_bus_solo.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

template <>
auto
reduce_mixer_state(
        audio_state const& st,
        set_bus_solo<audio::bus_direction::input> const& a) -> mixer::state
{
    auto mixer_state = st.mixer_state;

    BOOST_ASSERT(a.index < mixer_state.inputs.size());
    auto const& id = mixer_state.inputs[a.index];

    mixer_state.input_solo_id =
            id == mixer_state.input_solo_id ? mixer::bus_id{} : id;

    return mixer_state;
}

template auto
reduce(audio_state const&, set_bus_solo<audio::bus_direction::input> const&)
        -> audio_state;

} // namespace piejam::runtime::actions
