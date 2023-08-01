// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/io_direction.h>

#include <type_traits>
#include <utility>

namespace piejam
{

template <class T>
struct io_pair
{
    using type = T;

    T in;
    T out;

    constexpr io_pair() noexcept(std::is_nothrow_default_constructible_v<T>)
        : in()
        , out()
    {
    }

    constexpr io_pair(T const& i, T const& o) noexcept(
            std::is_nothrow_copy_constructible_v<T>)
        : in(i)
        , out(o)
    {
    }

    constexpr io_pair(T&& i, T&& o) noexcept(
            std::is_nothrow_move_constructible_v<T>)
        : in(std::move(i))
        , out(std::move(o))
    {
    }

    constexpr io_pair(io_pair const&) = default;
    constexpr io_pair(io_pair&&) noexcept = default;

    constexpr auto operator=(io_pair const&) -> io_pair& = default;
    constexpr auto operator=(io_pair&&) noexcept -> io_pair& = default;

    constexpr auto operator==(io_pair const&) const noexcept -> bool = default;

    constexpr auto get(io_direction const io_dir) const noexcept -> T const&
    {
        return io_dir == io_direction::input ? in : out;
    }

    constexpr auto get(io_direction const io_dir) noexcept -> T&
    {
        return io_dir == io_direction::input ? in : out;
    }
};

} // namespace piejam
