// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/engine_action_visitor.h>
#include <piejam/runtime/ui/action.h>

namespace piejam::runtime::actions
{

struct engine_action : ui::visitable_action_interface<engine_action_visitor>
{
};

template <class Action>
using visitable_engine_action = ui::visitable_action<Action, engine_action>;

} // namespace piejam::runtime::actions
