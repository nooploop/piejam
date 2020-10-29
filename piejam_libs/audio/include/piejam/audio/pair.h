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

#include <type_traits>
#include <utility>

namespace piejam::audio
{

// Utility class like std::pair. Just to make it easier to define and use
// a pair in audio context.
template <class T>
struct pair
{
    using type = T;

    T left;
    T right;

    constexpr pair() noexcept(std::is_nothrow_default_constructible_v<T>)
        : left()
        , right()
    {
    }

    explicit constexpr pair(T const& t) noexcept(
            std::is_nothrow_copy_constructible_v<T>)
        : left(t)
        , right(t)
    {
    }

    constexpr pair(T const& l, T const& r) noexcept(
            std::is_nothrow_copy_constructible_v<T>)
        : left(l)
        , right(r)
    {
    }

    constexpr pair(T&& l, T&& r) noexcept(
            std::is_nothrow_move_constructible_v<T>)
        : left(std::move(l))
        , right(std::move(r))
    {
    }

    constexpr pair(pair const&) = default;
    constexpr pair(pair&&) = default;

    constexpr auto operator=(pair const&) -> pair<T>& = default;
    constexpr auto operator=(pair &&) -> pair<T>& = default;
};

template <class T>
constexpr bool
operator==(pair<T> const& l, pair<T> const& r)
{
    return l.left == r.left && l.right == r.right;
}

template <class T>
constexpr bool
operator!=(pair<T> const& l, pair<T> const& r)
{
    return !(l == r);
}

} // namespace piejam::audio
