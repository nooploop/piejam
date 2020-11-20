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

#include <piejam/runtime/actions/add_bus.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

#include <fmt/format.h>

namespace piejam::runtime::actions
{

auto
add_bus::reduce(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    switch (direction)
    {
        case audio::bus_direction::input:
            add_mixer_bus<audio::bus_direction::input>(
                    new_st,
                    fmt::format("In {}", new_st.mixer_state.inputs.size() + 1),
                    type);
            break;

        case audio::bus_direction::output:
        {
            auto const bus_ids_size = new_st.mixer_state.outputs.size();
            add_mixer_bus<audio::bus_direction::output>(
                    new_st,
                    (bus_ids_size == 0 ? std::string("Main")
                                       : fmt::format("Aux {}", bus_ids_size)),
                    type);
            break;
        }
    }

    return st;
}

} // namespace piejam::runtime::actions
