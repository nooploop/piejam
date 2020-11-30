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

#include <piejam/runtime/actions/set_bus_name.h>

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <audio::bus_direction D>
auto
set_bus_name<D>::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.buses[mixer::bus_ids<D>(st.mixer_state)[bus]].name =
            name;

    return new_st;
}

template struct set_bus_name<audio::bus_direction::input>;
template struct set_bus_name<audio::bus_direction::output>;

} // namespace piejam::runtime::actions
