// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct ladspa_fx_action_visitor
    : ui::action_visitor_interface<
              load_ladspa_fx_plugin,
              delete_fx_module,
              delete_mixer_channel,
              reload_missing_plugins>
{
};

struct ladspa_fx_action
    : ui::visitable_action_interface<ladspa_fx_action_visitor>
{
};

template <class Action>
using visitable_ladspa_fx_action =
        ui::visitable_action<Action, ladspa_fx_action>;

} // namespace piejam::runtime::actions
