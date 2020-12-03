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

#include <cmath>
#include <concepts>
#include <limits>

namespace piejam::math
{

template <class T>
constexpr auto
pow3(T const x) noexcept -> T
{
    return x * x * x;
}

constexpr bool
is_power_of_two(std::integral auto x) noexcept
{
    return x > 0 && !(x & (x - 1));
}

constexpr auto
to_dB(float l) -> float
{
    static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");
    return l == 0.f ? -std::numeric_limits<float>::infinity()
                    : (std::log(l) / std::log(10.f)) * 20.f;
}

} // namespace piejam::math
