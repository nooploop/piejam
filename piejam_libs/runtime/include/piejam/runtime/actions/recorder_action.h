// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct recorder_action_visitor
    : ui::action_visitor_interface<
              start_recording,
              stop_recording,
              audio_engine_sync_update>
{
};

struct recorder_action : ui::visitable_action_interface<recorder_action_visitor>
{
};

template <class Action>
using visitable_recorder_action = ui::visitable_action<Action, recorder_action>;

} // namespace piejam::runtime::actions
