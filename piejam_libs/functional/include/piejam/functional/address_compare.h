// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/functional/operators.h>

#include <memory>

namespace piejam
{

namespace detail
{

template <class T>
struct unwrap_const_addressof
{
    using R = T const*;

    [[nodiscard]] constexpr auto operator()(T const& t) const noexcept -> R
    {
        return std::addressof(t);
    }

    [[nodiscard]] constexpr auto operator()(T const* t) const noexcept -> R
    {
        return t;
    }

    [[nodiscard]] constexpr auto
    operator()(std::reference_wrapper<T> const& t) const noexcept -> R
    {
        return std::addressof(t.get());
    }

    [[nodiscard]] constexpr auto
    operator()(std::reference_wrapper<T const> const& t) const noexcept -> R
    {
        return std::addressof(t.get());
    }
};

} // namespace detail

template <class T>
inline constexpr auto address_equal_to =
        equal_to<detail::unwrap_const_addressof<T>>;

template <class T>
inline constexpr auto address_not_equal_to =
        not_equal_to<detail::unwrap_const_addressof<T>>;

template <class T>
inline constexpr auto address_greater =
        greater<detail::unwrap_const_addressof<T>>;

template <class T>
inline constexpr auto address_greater_equal =
        greater_equal<detail::unwrap_const_addressof<T>>;

template <class T>
inline constexpr auto address_less = less<detail::unwrap_const_addressof<T>>;

template <class T>
inline constexpr auto address_less_equal =
        less_equal<detail::unwrap_const_addressof<T>>;

} // namespace piejam
