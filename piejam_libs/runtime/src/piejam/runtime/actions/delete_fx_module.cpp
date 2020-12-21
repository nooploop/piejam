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

#include <piejam/runtime/actions/delete_fx_module.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
delete_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    remove_fx_module(new_st, fx_mod_id);

    return new_st;
}

} // namespace piejam::runtime::actions
