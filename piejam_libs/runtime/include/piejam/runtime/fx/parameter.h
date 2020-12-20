// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
