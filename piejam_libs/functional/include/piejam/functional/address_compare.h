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

#include <functional>
#include <memory>

namespace piejam
{

template <class T, class Compare>
struct address_compare
{
    constexpr auto operator()(T const& l, T const& r) const noexcept -> bool
    {
        return Compare{}(std::addressof(l), std::addressof(r));
    }

    constexpr auto operator()(T const& l, T const* const r) const noexcept -> bool
    {
        return Compare{}(std::addressof(l), r);
    }

    constexpr auto operator()(T const* const l, T const& r) const noexcept -> bool
    {
        return Compare{}(l, std::addressof(r));
    }

    constexpr auto operator()(T const* const l, T const* const r) const noexcept -> bool
    {
        return Compare{}(l, r);
    }
};

template <class T>
using address_equal_to = address_compare<T, std::equal_to<>>;

template <class T>
using address_greater = address_compare<T, std::greater<>>;

template <class T>
using address_greater_equal = address_compare<T, std::greater_equal<>>;

template <class T>
using address_less = address_compare<T, std::less<>>;

template <class T>
using address_less_equal = address_compare<T, std::less_equal<>>;

template <class T>
using address_not_equal_to = address_compare<T, std::not_equal_to<>>;

} // namespace piejam
