// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>

namespace piejam::runtime
{

auto bool_parameter_value_to_string(bool) -> std::string;
auto int_parameter_value_to_string(int) -> std::string;
auto float_parameter_value_to_string(float) -> std::string;

} // namespace piejam::runtime
