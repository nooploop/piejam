// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

#include <nlohmann/json_fwd.hpp>

#include <vector>

namespace piejam::runtime::fx
{

void to_json(nlohmann::json&, parameter_midi_assignment const&);
void from_json(nlohmann::json const&, parameter_midi_assignment&);

} // namespace piejam::runtime::fx

namespace piejam::runtime::persistence
{

using fx_midi_assignments = std::vector<fx::parameter_midi_assignment>;

} // namespace piejam::runtime::persistence
