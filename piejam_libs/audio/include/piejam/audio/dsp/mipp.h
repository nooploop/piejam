// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <mipp.h>

#include <boost/assert.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/iteration/local.hpp>

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace piejam::audio::dsp
{

namespace detail
{

template <class T>
struct is_mipp_integral : std::false_type
{
};

template <>
struct is_mipp_integral<std::int8_t> : std::true_type
{
};

template <>
struct is_mipp_integral<std::int16_t> : std::true_type
{
};

template <>
struct is_mipp_integral<std::int32_t> : std::true_type
{
};

template <>
struct is_mipp_integral<std::int64_t> : std::true_type
{
};

template <>
struct is_mipp_integral<std::uint8_t> : std::true_type
{
};

template <>
struct is_mipp_integral<std::uint16_t> : std::true_type
{
};

template <>
struct is_mipp_integral<std::uint32_t> : std::true_type
{
};

template <>
struct is_mipp_integral<std::uint64_t> : std::true_type
{
};

} // namespace detail

template <class T>
constexpr bool is_mipp_integral_v =
        detail::is_mipp_integral<std::remove_cv_t<T>>::value;

template <class T>
concept mipp_integral = is_mipp_integral_v<T>;

template <class T>
concept mipp_signed_integral = mipp_integral<T> && std::is_signed_v<T>;

template <class T>
concept mipp_unsigned_integral = mipp_integral<T> && std::is_unsigned_v<T>;

template <class T>
concept mipp_number = mipp_integral<T> || std::floating_point<T>;

template <mipp_number T>
[[nodiscard]]
constexpr auto
mipp_lrot_n(mipp::Reg<T> reg, std::size_t n)
{
#define PIEJAM_LROT_N_MAX_LIMIT 16
    BOOST_ASSERT(n < mipp::N<T>());

    // clang-format off
    switch (n)
    {
#define BOOST_PP_LOCAL_LIMITS (1, PIEJAM_LROT_N_MAX_LIMIT - 1)
#define BOOST_PP_LOCAL_MACRO(I)                                                    \
        case BOOST_PP_SUB(PIEJAM_LROT_N_MAX_LIMIT, I):                             \
            reg = mipp::lrot(reg);                                                 \
            [[fallthrough]];
#include BOOST_PP_LOCAL_ITERATE()
#undef PIEJAM_LROT_N_MAX_LIMIT
        default:
            return reg;
    }
    // clang-format on
}

template <mipp_number T>
[[nodiscard]]
constexpr auto
mipp_fsqradd(mipp::Reg<T> reg, mipp::Reg<T> add)
{
    return mipp::fmadd(reg, reg, add);
}

} // namespace piejam::audio::dsp
