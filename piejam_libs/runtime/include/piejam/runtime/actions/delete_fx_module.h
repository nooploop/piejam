// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct delete_fx_module final
    : ui::cloneable_action<delete_fx_module, action>
    , visitable_engine_action<delete_fx_module>
{
    fx::module_id fx_mod_id;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
