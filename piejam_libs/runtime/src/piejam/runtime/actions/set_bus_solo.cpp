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

#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

template <audio::bus_direction D>
auto
set_bus_solo<D>::reduce(state const& st) const -> state
{
    static_assert(D == audio::bus_direction::input);

    auto new_st = st;

    BOOST_ASSERT(index < new_st.mixer_state.inputs->size());
    auto const& id = new_st.mixer_state.inputs.get()[index];

    new_st.mixer_state.input_solo_id =
            id == new_st.mixer_state.input_solo_id ? mixer::bus_id{} : id;

    return new_st;
}

template struct set_bus_solo<audio::bus_direction::input>;

} // namespace piejam::runtime::actions
