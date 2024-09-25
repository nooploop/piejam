// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/numeric/concepts.h>

#include <boost/assert.hpp>

#include <climits>

namespace piejam::numeric::bit
{

template <integral T>
[[nodiscard]]
constexpr auto
toggle(T const v, std::size_t const bit) noexcept -> T
{
    BOOST_ASSERT(bit < sizeof(T) * CHAR_BIT);
    return v ^ (T{1} << bit);
}

template <std::size_t Bit, integral T>
[[nodiscard]]
constexpr auto
toggle(T const v) noexcept -> T
{
    static_assert(Bit < sizeof(T) * CHAR_BIT);
    return v ^ (T{1} << Bit);
}

} // namespace piejam::numeric::bit
