// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <vector>

namespace piejam::runtime::fx
{

struct unavailable_ladspa
{
    audio::ladspa::plugin_id_t plugin_id;
    std::vector<fx::parameter_assignment> parameter_assignments;
};

} // namespace piejam::runtime::fx
