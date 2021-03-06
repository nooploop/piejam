// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <vector>

namespace piejam::runtime::actions
{

struct finalize_ladspa_fx_plugin_scan final
    : ui::cloneable_action<finalize_ladspa_fx_plugin_scan, action>
{
    std::vector<audio::ladspa::plugin_descriptor> plugins;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
