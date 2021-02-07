// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct request_info_update final
    : ui::cloneable_action<request_info_update, action>
    , visitable_engine_action<request_info_update>
{
    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
