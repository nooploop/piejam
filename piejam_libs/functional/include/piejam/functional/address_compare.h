// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/functional/operators.h>

#include <boost/hof/proj.hpp>

#include <memory>

namespace piejam
{

namespace detail
{

template <class T>
struct unwrap_const_addressof
{
    using R = T const*;

    [[nodiscard]]
    constexpr auto operator()(T const& t) const noexcept -> R
    {
        return std::addressof(t);
    }

    [[nodiscard]]
    constexpr auto operator()(T const* t) const noexcept -> R
    {
        return t;
    }

    [[nodiscard]]
    constexpr auto
    operator()(std::reference_wrapper<T> const& t) const noexcept -> R
    {
        return std::addressof(t.get());
    }

    [[nodiscard]]
    constexpr auto
    operator()(std::reference_wrapper<T const> const& t) const noexcept -> R
    {
        return std::addressof(t.get());
    }
};

} // namespace detail

template <class T>
inline constexpr auto address_equal_to =
        boost::hof::proj(detail::unwrap_const_addressof<T>{}, equal_to);

template <class T>
inline constexpr auto address_not_equal_to =
        boost::hof::proj(detail::unwrap_const_addressof<T>{}, not_equal_to);

template <class T>
inline constexpr auto address_greater =
        boost::hof::proj(detail::unwrap_const_addressof<T>{}, greater);

template <class T>
inline constexpr auto address_greater_equal =
        boost::hof::proj(detail::unwrap_const_addressof<T>{}, greater_equal);

template <class T>
inline constexpr auto address_less =
        boost::hof::proj(detail::unwrap_const_addressof<T>{}, less);

template <class T>
inline constexpr auto address_less_equal =
        boost::hof::proj(detail::unwrap_const_addressof<T>{}, less_equal);

} // namespace piejam
