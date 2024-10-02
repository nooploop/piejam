// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/numeric/bit.h>
#include <piejam/numeric/concepts.h>
#include <piejam/numeric/type_traits.h>

namespace piejam::numeric::intops
{

template <integral T>
constexpr std::size_t sign_bit = sizeof(T) * CHAR_BIT - 1;

template <integral Source>
[[nodiscard]]
constexpr auto
sign_map(Source const x) noexcept -> Source
{
    return x;
}

//! map integer ranges of different signedness:
//! [0 ; UMAX] <-> [SMIN ; SMAX]
template <integral Target, integral Source>
[[nodiscard]]
constexpr auto
sign_map(Source const x) noexcept -> Target
    requires(
            (is_signed_v<Target> != is_signed_v<Source>) &&
            (sizeof(Target) == sizeof(Source)))
{
    // toggle the sign bit
    return static_cast<Target>(bit::toggle<sign_bit<Source>>(x));
}

} // namespace piejam::numeric::intops
