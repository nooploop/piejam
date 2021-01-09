// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/device_action_visitor.h>
#include <piejam/runtime/ui/action.h>

namespace piejam::runtime::actions
{

struct device_action : ui::visitable_action_interface<device_action_visitor>
{
};

template <class Action>
using visitable_device_action = ui::visitable_action<Action, device_action>;

} // namespace piejam::runtime::actions
