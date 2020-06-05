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

#include <type_traits>

namespace piejam::numeric
{

template <class, std::size_t>
struct intx_t;

template <class T>
struct make_signed : std::make_signed<T>
{
};

template <class Integer, std::size_t Bits>
struct make_signed<intx_t<Integer, Bits>>
{
    using type = intx_t<std::make_signed_t<Integer>, Bits>;
};

template <class T>
using make_signed_t = typename make_signed<T>::type;

template <class T>
struct is_signed : std::is_signed<T>
{
};

template <class Integer, std::size_t Bits>
struct is_signed<intx_t<Integer, Bits>> : std::is_signed<Integer>
{
};

template <class T>
constexpr bool is_signed_v = is_signed<T>::value;

template <class T>
struct is_integral : std::is_integral<T>
{
};

template <class Integer, std::size_t Bits>
struct is_integral<intx_t<Integer, Bits>> : std::is_integral<Integer>
{
};

template <class T>
constexpr bool is_integral_v = is_integral<T>::value;

} // namespace piejam::numeric
