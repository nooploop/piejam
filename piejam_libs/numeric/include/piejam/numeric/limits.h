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

#include <limits>

namespace piejam::numeric
{

template <class T>
struct limits
{
    static constexpr auto min() noexcept -> T
    {
        return std::numeric_limits<T>::min();
    }
    static constexpr auto max() noexcept -> T
    {
        return std::numeric_limits<T>::max();
    }
};

template <>
struct limits<int24_t>
{
    static constexpr auto min() noexcept -> int24_t { return -8388608; }
    static constexpr auto max() noexcept -> int24_t { return 8388607; }
};

template <>
struct limits<uint24_t>
{
    static constexpr auto min() noexcept -> uint24_t { return 0; }
    static constexpr auto max() noexcept -> uint24_t { return 16777215; }
};

} // namespace piejam::numeric
