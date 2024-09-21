// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/parameter_value_to_string.h>

#include <fmt/format.h>

namespace piejam::runtime
{

auto
bool_parameter_value_to_string(bool x) -> std::string
{
    using namespace std::string_literals;
    return x ? "on"s : "off"s;
}

auto
int_parameter_value_to_string(int x) -> std::string
{
    return std::to_string(x);
}

auto
float_parameter_value_to_string(float x) -> std::string
{
    return fmt::format("{:.2f}", x);
}

} // namespace piejam::runtime
