// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    constexpr auto operator=(pair const&) -> pair& = default;
    constexpr auto operator=(pair&&) -> pair& = default;

    constexpr auto operator==(pair const&) const noexcept -> bool = default;
};

} // namespace piejam::audio
