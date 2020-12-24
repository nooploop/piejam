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

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct move_fx_module_left final
    : ui::cloneable_action<move_fx_module_left, action>
    , visitable_engine_action<move_fx_module_left>
{
    fx::module_id fx_mod_id;

    auto reduce(state const&) const -> state override;
};

struct move_fx_module_right final
    : ui::cloneable_action<move_fx_module_right, action>
    , visitable_engine_action<move_fx_module_right>
{
    fx::module_id fx_mod_id;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions