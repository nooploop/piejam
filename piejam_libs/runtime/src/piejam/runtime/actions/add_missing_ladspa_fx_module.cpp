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

#include <piejam/runtime/actions/add_missing_ladspa_fx_module.h>

#include <piejam/runtime/audio_state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
add_missing_ladspa_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    BOOST_ASSERT(fx_chain_bus != mixer::bus_id{});
    runtime::add_missing_ladspa_fx_module(
            new_st,
            fx_chain_bus,
            missing_id,
            name);

    return new_st;
}

} // namespace piejam::runtime::actions
