// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id_hash.h>
#include <piejam/ladspa/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <unordered_map>

namespace piejam::runtime::fx
{

using ladspa_instances =
        std::unordered_map<ladspa::instance_id, ladspa::plugin_descriptor>;

} // namespace piejam::runtime::fx
