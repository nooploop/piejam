// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/ladspa_fx_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct reload_missing_plugins final
    : ui::cloneable_action<reload_missing_plugins, action>
    , visitable_ladspa_fx_action<reload_missing_plugins>
{
};

} // namespace piejam::runtime::actions
