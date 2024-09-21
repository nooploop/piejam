// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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

    template <std::convertible_to<T> I, std::convertible_to<T> O>
    constexpr io_pair(I&& i, O&& o) noexcept(noexcept(T{
            std::forward<I>(i)}) && noexcept(T{std::forward<O>(o)}))
        : in{std::forward<I>(i)}
        , out{std::forward<O>(o)}
    {
    }

    constexpr io_pair(io_pair const&) = default;
    constexpr io_pair(io_pair&&) noexcept = default;

    constexpr auto operator=(io_pair const&) -> io_pair& = default;
    constexpr auto operator=(io_pair&&) noexcept -> io_pair& = default;

    auto operator==(io_pair const&) const noexcept -> bool = default;

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
