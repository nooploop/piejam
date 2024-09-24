// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/numeric/bitops.h>
#include <piejam/numeric/type_traits.h>

namespace piejam::numeric::intops
{

template <class Source>
[[nodiscard]]
constexpr auto
sign_map(Source const x) noexcept -> Source
{
    static_assert(
            is_integral_v<Source>,
            "sign_map can only be applied to integer types");
    return x;
}

//! map integer ranges of different signedness:
//! [0 ; UMAX] <-> [SMIN ; SMAX]
template <class Target, class Source>
[[nodiscard]]
constexpr auto
sign_map(Source const x) noexcept
        -> std::enable_if_t<is_signed_v<Target> != is_signed_v<Source>, Target>
{
    static_assert(
            is_integral_v<Target>,
            "sign_map: Target must be an integer type");
    static_assert(
            is_integral_v<Source>,
            "sign_map: Source must be an integer type");
    static_assert(
            sizeof(Target) == sizeof(Source),
            "sign_map can only be applied to types of same size");

    // toggle the sign bit
    return bitops::toggle(x, sizeof(Source) * 8u - 1);
}

} // namespace piejam::numeric::intops
