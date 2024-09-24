// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/numeric/intx.h>

#include <boost/endian/conversion.hpp>

namespace piejam::numeric::endian
{

template <class T>
[[nodiscard]]
constexpr auto
reverse_bytes(intx_t<T, 24> x) noexcept -> intx_t<T, 24>
{
    return ((x.value & 0xff0000) >> 16) | (x.value & 0xff00) |
           ((x.value & 0xff) << 16);
}

template <class T>
[[nodiscard]]
constexpr auto
little_to_native(T x) -> T
{
    return boost::endian::little_to_native(x);
}

template <class T>
[[nodiscard]]
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
[[nodiscard]]
constexpr auto
big_to_native(T x) -> T
{
    return boost::endian::big_to_native(x);
}

template <class T>
[[nodiscard]]
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
[[nodiscard]]
constexpr auto
native_to_little(T x) -> T
{
    return boost::endian::native_to_little(x);
}

template <class T>
[[nodiscard]]
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
[[nodiscard]]
constexpr auto
native_to_big(T x) -> T
{
    return boost::endian::big_to_native(x);
}

template <class T>
[[nodiscard]]
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
