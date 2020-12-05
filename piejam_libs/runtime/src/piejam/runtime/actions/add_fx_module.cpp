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

#include <piejam/runtime/actions/add_fx_module.h>

#include <piejam/functional/overload.h>
#include <piejam/runtime/audio_state.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
add_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    BOOST_ASSERT(st.fx_chain_bus != mixer::bus_id{});
    std::visit(
            overload{
                    [&](fx::internal fx_type) {
                        runtime::add_fx_module(
                                new_st,
                                st.fx_chain_bus,
                                fx_type);
                    },
                    [](audio::ladspa::plugin_descriptor const&) {}},
            reg_item);

    return new_st;
}

} // namespace piejam::runtime::actions
