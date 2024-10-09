// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

#include <functional>
#include <string_view>

namespace piejam::runtime::fx
{

using get_parameter_name = std::function<std::string_view(parameter_id const&)>;

} // namespace piejam::runtime::fx
