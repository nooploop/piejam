// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>

#include <cmath>
#include <concepts>
#include <limits>

namespace piejam::math
{

template <class T>
[[nodiscard]] constexpr auto
pow3(T const x) noexcept -> T
{
    return x * x * x;
}

template <std::floating_point T>
constexpr inline T negative_inf = -std::numeric_limits<T>::infinity();

template <std::floating_point T>
[[nodiscard]] constexpr auto
to_dB(T const log, T const min_log = T{}) -> T
{
    static_assert(std::numeric_limits<T>::is_iec559, "IEEE 754 required");
    return log <= min_log ? negative_inf<T> : std::log10(log) * T{20};
}

template <std::floating_point T>
[[nodiscard]] constexpr auto
from_dB(T const dB, T const min_dB = negative_inf<T>) -> T
{
    return dB <= min_dB ? T{} : std::pow(T{10}, dB / T{20});
}

template <class T>
[[nodiscard]] constexpr auto
clamp(T v, T const min, T const max) -> T
    requires(std::is_arithmetic_v<T>)
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

template <std::floating_point T>
[[nodiscard]] constexpr auto
linear_map(
        T const v,
        T const src_lo,
        T const src_hi,
        T const dst_lo,
        T const dst_hi) -> T
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

} // namespace piejam::math
