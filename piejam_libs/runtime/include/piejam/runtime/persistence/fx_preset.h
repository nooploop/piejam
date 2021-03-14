// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

#include <nlohmann/json_fwd.hpp>

#include <vector>

namespace piejam::runtime::fx
{

void to_json(nlohmann::json&, parameter_value_assignment const&);
void from_json(nlohmann::json const&, parameter_value_assignment&);

} // namespace piejam::runtime::fx

namespace piejam::runtime::persistence
{

using fx_preset = std::vector<fx::parameter_value_assignment>;

} // namespace piejam::runtime::persistence
