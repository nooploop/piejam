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

#include <piejam/numeric/type_traits.h>

#include <cassert>

namespace piejam::numeric::bitops
{

template <class T>
constexpr auto
set(T const v, std::size_t const bit) noexcept -> T
{
    static_assert(is_integral_v<T>);
    assert(bit < sizeof(T) * 8u);
    return v | (T{1} << bit);
}

template <class T, std::size_t Bit>
constexpr auto
set(T const v) noexcept -> T
{
    static_assert(is_integral_v<T>);
    static_assert(Bit < sizeof(T) * 8u);
    return v | (T{1} << Bit);
}

template <class T>
constexpr auto
reset(T const v, std::size_t const bit) noexcept -> T
{
    static_assert(is_integral_v<T>);
    assert(bit < sizeof(T) * 8u);
    return v & ~(T{1} << bit);
}

template <class T, std::size_t Bit>
constexpr auto
reset(T const v) noexcept -> T
{
    static_assert(is_integral_v<T>);
    static_assert(Bit < sizeof(T) * 8u);
    return v & ~(T{1} << Bit);
}

template <class T>
constexpr auto
toggle(T const v, std::size_t const bit) noexcept -> T
{
    static_assert(is_integral_v<T>);
    assert(bit < sizeof(T) * 8u);
    return v ^ (T{1} << bit);
}

template <class T, std::size_t Bit>
constexpr auto
toggle(T const v) noexcept -> T
{
    static_assert(is_integral_v<T>);
    static_assert(Bit < sizeof(T) * 8u);
    return v ^ (T{1} << Bit);
}

} // namespace piejam::numeric::bitops
