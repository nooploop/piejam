// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct device_action_visitor
    : ui::action_visitor_interface<
              apply_app_config,
              refresh_devices,
              initiate_device_selection,
              select_samplerate,
              select_period_size,
              select_period_count,
              activate_midi_device,
              deactivate_midi_device>
{
};

struct device_action : ui::visitable_action_interface<device_action_visitor>
{
};

template <class Action>
using visitable_device_action = ui::visitable_action<Action, device_action>;

} // namespace piejam::runtime::actions
