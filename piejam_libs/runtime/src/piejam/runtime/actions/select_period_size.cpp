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

#include <piejam/runtime/actions/select_period_size.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <>
auto
reduce_period_size(audio_state const& st, select_period_size const& a)
        -> audio::samplerate_t
{
    return period_sizes_from_state(st)[a.index];
}

template auto reduce(audio_state const&, select_period_size const&)
        -> audio_state;

} // namespace piejam::runtime::actions