// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>
#include <boost/hof/lift.hpp>

#include <cmath>
#include <concepts>
#include <functional>
#include <limits>

namespace piejam::math
{

template <std::floating_point T>
constexpr T negative_inf = -std::numeric_limits<T>::infinity();

template <std::floating_point T>
[[nodiscard]]
constexpr auto
to_dB(T log, T min_log = T{}) -> T
{
    static_assert(std::numeric_limits<T>::is_iec559, "IEEE 754 required");
    return log <= min_log ? negative_inf<T> : std::log10(log) * T{20};
}

template <std::floating_point T>
[[nodiscard]]
constexpr auto
from_dB(T dB, T min_dB = negative_inf<T>) -> T
{
    return dB <= min_dB ? T{} : std::pow(T{10}, dB / T{20});
}

template <class T>
    requires(std::is_arithmetic_v<T>)
[[nodiscard]]
constexpr auto
clamp(T v, T min, T max) -> T
{
    if (v < min)
    {
        v = min;
    }

    if (v > max)
    {
        v = max;
    }

    return v;
}

template <class T, std::invocable<T> P>
[[nodiscard]]
constexpr auto
flush_to_zero_if(T value, P&& p) -> T
{
    return static_cast<T>(!std::invoke(std::forward<P>(p), value)) * value;
}

template <std::floating_point T>
[[nodiscard]]
constexpr auto
linear_map(T v, T src_lo, T src_hi, T dst_lo, T dst_hi) -> T
{
    return ((v - src_lo) / (src_hi - src_lo)) * (dst_hi - dst_lo) + dst_lo;
}

template <std::integral T>
constexpr auto
pos_mod(T x, T y)
{
    auto n = x % y;
    return n < 0 ? n + y : n;
}

inline constexpr auto abs = BOOST_HOF_LIFT(std::abs);

template <std::integral T, std::integral N>
constexpr auto
round_down_to_multiple(T x, N n) -> T
{
    return (x / n) * n;
}

} // namespace piejam::math
