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

#include <piejam/numeric/intx.h>

#include <boost/endian/conversion.hpp>

namespace piejam::numeric::endian
{

template <class T>
constexpr auto
reverse_bytes(intx_t<T, 24> x) noexcept -> intx_t<T, 24>
{
    return ((x.value & 0xff0000) >> 16) | (x.value & 0xff00) |
           ((x.value & 0xff) << 16);
}

template <class T>
constexpr auto
little_to_native(T x) -> T
{
    return boost::endian::little_to_native(x);
}

template <class T>
constexpr auto
little_to_native(intx_t<T, 24> x) -> intx_t<T, 24>
{
#if BOOST_ENDIAN_LITTLE_BYTE
    return x;
#else
    return reverse_bytes(x);
#endif
}

template <class T>
constexpr auto
big_to_native(T x) -> T
{
    return boost::endian::big_to_native(x);
}

template <class T>
constexpr auto
big_to_native(intx_t<T, 24> x) -> intx_t<T, 24>
{
#if BOOST_ENDIAN_BIG_BYTE
    return x;
#else
    return reverse_bytes(x);
#endif
}

template <class T>
constexpr auto
native_to_little(T x) -> T
{
    return boost::endian::native_to_little(x);
}

template <class T>
constexpr auto
native_to_little(intx_t<T, 24> x) -> intx_t<T, 24>
{
#if BOOST_ENDIAN_LITTLE_BYTE
    return x;
#else
    return reverse_bytes(x);
#endif
}

template <class T>
constexpr auto
native_to_big(T x) -> T
{
    return boost::endian::big_to_native(x);
}

template <class T>
constexpr auto
native_to_big(intx_t<T, 24> x) -> intx_t<T, 24>
{
#if BOOST_ENDIAN_BIG_BYTE
    return x;
#else
    return reverse_bytes(x);
#endif
}

} // namespace piejam::numeric::endian
