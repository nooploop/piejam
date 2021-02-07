// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct persistence_action_visitor
    : ui::action_visitor_interface<
              load_app_config,
              save_app_config,
              load_session,
              save_session>
{
};

struct persistence_action
    : ui::visitable_action_interface<persistence_action_visitor>
{
};

template <class Action>
using visitable_persistence_action =
        ui::visitable_action<Action, persistence_action>;

} // namespace piejam::runtime::actions
