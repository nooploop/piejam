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
[[nodiscard]] constexpr auto
linear_to_dB(T const linear, T const min_linear = T{}) -> T
{
    static_assert(std::numeric_limits<T>::is_iec559, "IEEE 754 required");
    return linear <= min_linear ? -std::numeric_limits<T>::infinity()
                                : std::log10(linear) * 20.f;
}

template <std::floating_point T>
[[nodiscard]] constexpr auto
dB_to_linear(T const dB) -> T
{
    return dB == -std::numeric_limits<T>::infinity()
                   ? T{}
                   : std::pow(T{10}, dB / T{20});
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

} // namespace piejam::math
