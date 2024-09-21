// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

#include <vector>

namespace piejam::runtime::persistence
{

using fx_preset = std::vector<fx::parameter_value_assignment>;

} // namespace piejam::runtime::persistence
