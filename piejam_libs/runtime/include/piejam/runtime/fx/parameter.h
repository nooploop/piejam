// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>

#include <variant>

namespace piejam::runtime::fx
{

struct parameter_value_to_string
{
    using float_to_string = std::string (*)(float);
    using int_to_string = std::string (*)(int);
    using bool_to_string = std::string (*)(bool);
    using to_string_f =
            std::variant<float_to_string, int_to_string, bool_to_string>;

    parameter_value_to_string() noexcept;
    parameter_value_to_string(float_to_string) noexcept;
    parameter_value_to_string(int_to_string) noexcept;
    parameter_value_to_string(bool_to_string) noexcept;

    auto operator()(float) const -> std::string;
    auto operator()(int) const -> std::string;
    auto operator()(bool) const -> std::string;

private:
    to_string_f m_to_string;
};

struct parameter
{
    boxed_string name;
    parameter_value_to_string value_to_string;
};

} // namespace piejam::runtime::fx
