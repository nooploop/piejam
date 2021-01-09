// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct device_action_visitor
    : ui::action_visitor_interface<
              apply_app_config,
              refresh_devices,
              initiate_device_selection,
              select_samplerate,
              select_period_size>
{
};

} // namespace piejam::runtime::actions
