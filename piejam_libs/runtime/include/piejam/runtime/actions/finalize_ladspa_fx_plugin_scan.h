// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/ladspa/plugin_descriptor.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <vector>

namespace piejam::runtime::actions
{

struct finalize_ladspa_fx_plugin_scan final
    : ui::cloneable_action<finalize_ladspa_fx_plugin_scan, reducible_action>
{
    std::vector<ladspa::plugin_descriptor> plugins;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
