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

#include <piejam/runtime/parameter/float_.h>

#include <cmath>

namespace piejam::runtime::parameter
{

inline constexpr auto
to_normalized_linear(float_ const& p, float const value) -> float
{
    return (value - p.min) / (p.max - p.min);
}

inline constexpr auto
from_normalized_linear(float_ const& p, float const norm_value) -> float
{
    return norm_value * (p.max - p.min) + p.min;
}

inline constexpr auto
to_normalized_log(float_ const& p, float const value) -> float
{
    constexpr float const one_div_log_10 = 1.f / std::log(10.f);
    float const max = std::log(p.max) * one_div_log_10;
    float const min = std::log(p.min) * one_div_log_10;
    float const value_log = std::log(value) * one_div_log_10;
    return (value_log - min) / (max - min);
}

inline constexpr auto
from_normalized_log(float_ const& p, float const norm_value) -> float
{
    constexpr float const one_div_log_10 = 1.f / std::log(10.f);
    float const max_db = std::log(p.max) * one_div_log_10;
    float const min_db = std::log(p.min) * one_div_log_10;
    float const value_log = norm_value * (max_db - min_db) + min_db;
    return std::pow(10.f, value_log);
}

inline constexpr auto
to_normalized_db(float_ const& p, float const value) -> float
{
    constexpr float const one_div_log_10_x_20 = 20.f / std::log(10.f);
    float const max_db = std::log(p.max) * one_div_log_10_x_20;
    float const min_db = std::log(p.min) * one_div_log_10_x_20;
    float const value_db = std::log(value) * one_div_log_10_x_20;
    return (value_db - min_db) / (max_db - min_db);
}

template <class DbInterval>
constexpr auto
to_normalized_db(float_ const&, float const value) -> float
{
    constexpr float const one_div_log_10_x_20 = 20.f / std::log(10.f);
    float const value_db = std::log(value) * one_div_log_10_x_20;
    return (value_db - DbInterval::min) / (DbInterval::max - DbInterval::min);
}

inline constexpr auto
from_normalized_db(float_ const& p, float const norm_value) -> float
{
    constexpr float const one_div_log_10_x_20 = 20.f / std::log(10.f);
    float const max_db = std::log(p.max) * one_div_log_10_x_20;
    float const min_db = std::log(p.min) * one_div_log_10_x_20;
    float const value_db = norm_value * (max_db - min_db) + min_db;
    return std::pow(10.f, value_db / 20.f);
}

template <class DbInterval>
constexpr auto
from_normalized_db(float_ const&, float const norm_value) -> float
{
    float const value_db =
            norm_value * (DbInterval::max - DbInterval::min) + DbInterval::min;
    return std::pow(10.f, value_db / 20.f);
}

} // namespace piejam::runtime::parameter
