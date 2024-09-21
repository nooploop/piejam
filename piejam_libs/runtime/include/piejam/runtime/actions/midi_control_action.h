// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct midi_control_action_visitor
    : ui::action_visitor_interface<
              refresh_midi_devices,
              save_app_config,
              apply_app_config,
              request_info_update>
{
};

struct midi_control_action
    : ui::visitable_action_interface<midi_control_action_visitor>
{
};

template <class Action>
using visitable_midi_control_action =
        ui::visitable_action<Action, midi_control_action>;

} // namespace piejam::runtime::actions
