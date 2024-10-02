// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <bit>
#include <concepts>
#include <cstdint>

namespace piejam
{

template <std::signed_integral T>
constexpr auto
switch_cast(T x) noexcept
{
    return x;
}

constexpr auto
switch_cast(float x) noexcept
{
    return std::bit_cast<std::int32_t>(x);
}

constexpr auto
switch_cast(double x) noexcept
{
    return std::bit_cast<std::int64_t>(x);
}

} // namespace piejam
