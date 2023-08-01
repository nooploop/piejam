// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct move_fx_module_up final
    : ui::cloneable_action<move_fx_module_up, reducible_action>
    , visitable_engine_action<move_fx_module_up>
{
    [[nodiscard]] auto reduce(state const&) const -> state override;
};

struct move_fx_module_down final
    : ui::cloneable_action<move_fx_module_down, reducible_action>
    , visitable_engine_action<move_fx_module_down>
{
    [[nodiscard]] auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
