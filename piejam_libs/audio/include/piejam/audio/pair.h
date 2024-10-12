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

    template <class U>
    explicit constexpr pair(U const& u) noexcept(
            std::is_nothrow_constructible_v<T, U>)
        : left(u)
        , right(u)
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

    constexpr ~pair() = default;

    constexpr auto operator=(pair const&) -> pair& = default;
    constexpr auto operator=(pair&&) -> pair& = default;

    constexpr auto operator==(pair const&) const noexcept -> bool = default;
};

template <std::size_t I, class T>
    requires(I < 2)
auto
get(pair<T> const& p) -> T const&
{
    return I == 0 ? p.left : p.right;
}

template <std::size_t I, class T>
    requires(I < 2)
auto
get(pair<T>& p) -> T&
{
    return I == 0 ? p.left : p.right;
}

template <std::size_t I, class T>
    requires(I < 2)
auto
get(pair<T>&& p) -> T&&
{
    return I == 0 ? std::move(p).left : std::move(p).right;
}

} // namespace piejam::audio

namespace std
{

template <class T>
struct tuple_size<piejam::audio::pair<T>>
    : std::integral_constant<std::size_t, 2>
{
};

template <std::size_t I, class T>
struct tuple_element<I, piejam::audio::pair<T>>
{
    using type = T;
};

} // namespace std
