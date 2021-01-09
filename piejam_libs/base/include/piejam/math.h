// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cmath>
#include <concepts>
#include <limits>

namespace piejam::math
{

template <class T>
constexpr auto
pow3(T const x) noexcept -> T
{
    return x * x * x;
}

constexpr bool
is_power_of_two(std::integral auto x) noexcept
{
    return x > 0 && !(x & (x - 1));
}

constexpr auto
to_dB(float l) -> float
{
    static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");
    return l == 0.f ? -std::numeric_limits<float>::infinity()
                    : (std::log(l) / std::log(10.f)) * 20.f;
}

} // namespace piejam::math
