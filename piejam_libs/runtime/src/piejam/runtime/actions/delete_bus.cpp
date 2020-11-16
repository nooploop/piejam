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

#include <piejam/runtime/actions/delete_bus.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

#include <fmt/format.h>

namespace piejam::runtime::actions
{

template <>
auto
reduce_mixer_state(audio_state const& st, delete_bus const& a) -> mixer::state
{
    auto mixer_state = st.mixer_state;

    if (a.direction == audio::bus_direction::input)
    {
        auto& inputs = mixer_state.inputs;
        BOOST_ASSERT(a.bus < inputs.size());
        auto const& id = inputs[a.bus];
        if (mixer_state.input_solo_id == id)
            mixer_state.input_solo_id = mixer::bus_id{};
        mixer::remove_bus(mixer_state.buses, mixer_state.inputs, a.bus);
    }
    else
    {
        auto& outputs = mixer_state.outputs;
        BOOST_ASSERT(a.direction == audio::bus_direction::output);
        BOOST_ASSERT(a.bus < outputs.size());
        mixer::remove_bus(mixer_state.buses, mixer_state.outputs, a.bus);
    }

    return mixer_state;
}

template auto reduce(audio_state const&, delete_bus const&) -> audio_state;

} // namespace piejam::runtime::actions
