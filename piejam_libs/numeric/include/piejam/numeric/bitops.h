// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/numeric/type_traits.h>

#include <cassert>

namespace piejam::numeric::bitops
{

template <class T>
[[nodiscard]] constexpr auto
set(T const v, std::size_t const bit) noexcept -> T
{
    static_assert(is_integral_v<T>);
    assert(bit < sizeof(T) * 8u);
    return v | (T{1} << bit);
}

template <class T, std::size_t Bit>
[[nodiscard]] constexpr auto
set(T const v) noexcept -> T
{
    static_assert(is_integral_v<T>);
    static_assert(Bit < sizeof(T) * 8u);
    return v | (T{1} << Bit);
}

template <class T>
[[nodiscard]] constexpr auto
reset(T const v, std::size_t const bit) noexcept -> T
{
    static_assert(is_integral_v<T>);
    assert(bit < sizeof(T) * 8u);
    return v & ~(T{1} << bit);
}

template <class T, std::size_t Bit>
[[nodiscard]] constexpr auto
reset(T const v) noexcept -> T
{
    static_assert(is_integral_v<T>);
    static_assert(Bit < sizeof(T) * 8u);
    return v & ~(T{1} << Bit);
}

template <class T>
[[nodiscard]] constexpr auto
toggle(T const v, std::size_t const bit) noexcept -> T
{
    static_assert(is_integral_v<T>);
    assert(bit < sizeof(T) * 8u);
    return v ^ (T{1} << bit);
}

template <class T, std::size_t Bit>
[[nodiscard]] constexpr auto
toggle(T const v) noexcept -> T
{
    static_assert(is_integral_v<T>);
    static_assert(Bit < sizeof(T) * 8u);
    return v ^ (T{1} << Bit);
}

} // namespace piejam::numeric::bitops
