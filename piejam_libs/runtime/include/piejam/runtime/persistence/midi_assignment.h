// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <nlohmann/json_fwd.hpp>

#include <vector>

namespace piejam::runtime
{

void to_json(nlohmann::json&, midi_assignment const&);
void from_json(nlohmann::json const&, midi_assignment&);

} // namespace piejam::runtime
