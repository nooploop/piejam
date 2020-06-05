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

#include <piejam/numeric/bitops.h>
#include <piejam/numeric/type_traits.h>

namespace piejam::numeric::intops
{

template <class Source>
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
