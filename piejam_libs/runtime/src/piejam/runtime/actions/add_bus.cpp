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

template <>
auto
reduce_mixer_state(audio_state const& st, add_bus const& a) -> mixer::state
{
    auto mixer_state = st.mixer_state;

    auto& bus_ids = a.direction == audio::bus_direction::input
                            ? mixer_state.inputs
                            : mixer_state.outputs;
    auto name = a.direction == audio::bus_direction::input
                        ? fmt::format("In {}", bus_ids.size() + 1)
                        : (bus_ids.size() == 0
                                   ? std::string("Main")
                                   : fmt::format("Aux {}", bus_ids.size()));
    mixer::add_bus(
            mixer_state.buses,
            bus_ids,
            mixer::bus{.name = std::move(name), .type = a.type});

    return mixer_state;
}

template auto reduce(audio_state const&, add_bus const&) -> audio_state;

} // namespace piejam::runtime::actions
