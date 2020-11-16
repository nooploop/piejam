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

    if (a.direction == audio::bus_direction::input)
    {
        auto name = fmt::format("In {}", mixer_state.inputs.size() + 1);
        mixer::add_bus(
                mixer_state.buses,
                mixer_state.inputs,
                mixer::bus{.name = std::move(name), .type = a.type});
    }
    else
    {
        BOOST_ASSERT(a.direction == audio::bus_direction::output);
        auto const old_size = mixer_state.outputs.size();
        BOOST_ASSERT(a.type == audio::bus_type::stereo);
        auto name = old_size == 0 ? std::string("Main")
                                  : fmt::format("Aux {}", old_size);
        mixer::add_bus(
                mixer_state.buses,
                mixer_state.outputs,
                mixer::bus{.name = std::move(name), .type = a.type});
    }

    return mixer_state;
}

template auto reduce(audio_state const&, add_bus const&) -> audio_state;

} // namespace piejam::runtime::actions
