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

#include <piejam/runtime/actions/set_bus_pan_balance.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <audio::bus_direction D>
static auto
reduce_mixer_state(audio_state const& st, set_bus_pan_balance<D> const& a)
        -> mixer::state
{
    return mixer::update_bus_field<D>(
            st.mixer_state,
            a.index,
            &mixer::bus::pan_balance,
            a.pan_balance);
}

template auto
reduce(audio_state const&,
       set_bus_pan_balance<audio::bus_direction::input> const&) -> audio_state;

template auto
reduce(audio_state const&,
       set_bus_pan_balance<audio::bus_direction::output> const&) -> audio_state;

} // namespace piejam::runtime::actions
