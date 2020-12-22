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

#include <piejam/runtime/actions/move_fx_module.h>

#include <piejam/runtime/state.h>

#include <algorithm>

namespace piejam::runtime::actions
{

auto
move_fx_module_left::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.buses.update(st.fx_chain_bus, [this](mixer::bus& bus) {
        bus.fx_chain.update([this](fx::chain_t& fx_chain) {
            if (auto it = std::ranges::find(fx_chain, fx_mod_id);
                it != fx_chain.end() && it != fx_chain.begin())
            {
                std::iter_swap(it, std::prev(it));
            }
        });
    });

    return new_st;
}

auto
move_fx_module_right::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.buses.update(st.fx_chain_bus, [this](mixer::bus& bus) {
        bus.fx_chain.update([this](fx::chain_t& fx_chain) {
            if (auto it = std::ranges::find(fx_chain, fx_mod_id);
                it != fx_chain.end() && std::next(it) != fx_chain.end())
            {
                std::iter_swap(it, std::next(it));
            }
        });
    });

    return new_st;
}

} // namespace piejam::runtime::actions
