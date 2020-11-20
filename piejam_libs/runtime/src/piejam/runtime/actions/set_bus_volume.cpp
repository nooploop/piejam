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

#include <piejam/runtime/actions/set_bus_volume.h>

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <audio::bus_direction D>
auto
set_bus_volume<D>::reduce(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    mixer::bus const& bus = mixer::get_bus<D>(new_st.mixer_state, index);
    new_st.float_params.set(bus.volume, volume);

    return new_st;
}

template struct set_bus_volume<audio::bus_direction::input>;
template struct set_bus_volume<audio::bus_direction::output>;

} // namespace piejam::runtime::actions
