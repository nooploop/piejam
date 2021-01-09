// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action.h>
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

} // namespace piejam::runtime::actions
