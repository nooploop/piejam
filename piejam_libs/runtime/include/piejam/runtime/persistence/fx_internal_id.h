// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

#include <nlohmann/json_fwd.hpp>

namespace piejam::runtime::fx
{

void to_json(nlohmann::json&, internal_id const&);
void from_json(nlohmann::json const&, internal_id&);

} // namespace piejam::runtime::fx

namespace piejam::runtime::persistence
{

void register_internal_fx(fx::internal_id, std::string name);

} // namespace piejam::runtime::persistence
