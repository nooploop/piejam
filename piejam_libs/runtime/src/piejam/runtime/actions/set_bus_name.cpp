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

auto
set_bus_name::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    switch (bus_direction)
    {
        case audio::bus_direction::input:
            assert(bus < new_st.mixer_state.inputs.size());
            new_st.mixer_state.inputs[bus].name = name;
            break;

        case audio::bus_direction::output:
            assert(bus == 0);
            new_st.mixer_state.output.name = name;
            break;

        default:
            assert(false);
            break;
    }

    return new_st;
}

} // namespace piejam::runtime::actions
