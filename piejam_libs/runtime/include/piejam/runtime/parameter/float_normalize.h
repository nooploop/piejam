// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/math.h>
#include <piejam/runtime/parameter/float_.h>

#include <cmath>

namespace piejam::runtime::parameter
{

[[nodiscard]] inline constexpr auto
to_normalized_linear(float_ const& p, float const value) -> float
{
    return math::linear_map(value, p.min, p.max, 0.f, 1.f);
}

[[nodiscard]] inline constexpr auto
from_normalized_linear(float_ const& p, float const norm_value) -> float
{
    return math::linear_map(norm_value, 0.f, 1.f, p.min, p.max);
}

[[nodiscard]] inline constexpr auto
to_normalized_log(float_ const& p, float const value) -> float
{
    return math::linear_map(
            std::log(value),
            std::log(p.min),
            std::log(p.max),
            0.f,
            1.f);
}

[[nodiscard]] inline constexpr auto
from_normalized_log(float_ const& p, float const norm_value) -> float
{
    return std::pow(p.min, 1.f - norm_value) * std::pow(p.max, norm_value);
}

[[nodiscard]] inline constexpr auto
to_normalized_dB(float_ const& p, float const value) -> float
{
    return to_normalized_log(p, value);
}

template <class Interval_dB>
[[nodiscard]] constexpr auto
to_normalized_dB(float_ const&, float const value) -> float
{
    constexpr float const twenty_div_log_10 = 20.f / std::log(10.f);
    float const value_dB = std::log(value) * twenty_div_log_10;
    return (value_dB - Interval_dB::min) / (Interval_dB::max - Interval_dB::min);
}

[[nodiscard]] inline constexpr auto
from_normalized_dB(float_ const& p, float const norm_value) -> float
{
    return from_normalized_log(p, norm_value);
}

template <class Interval_dB>
[[nodiscard]] constexpr auto
from_normalized_dB(float_ const&, float const norm_value) -> float
{
    float const value_dB =
            norm_value * (Interval_dB::max - Interval_dB::min) + Interval_dB::min;
    return std::pow(10.f, value_dB / 20.f);
}

} // namespace piejam::runtime::parameter
