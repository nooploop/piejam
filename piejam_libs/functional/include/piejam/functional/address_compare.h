// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>
#include <memory>

namespace piejam
{

namespace detail
{

template <class T>
struct unwrap_const_addressof
{
    constexpr auto operator()(T const& t) const noexcept -> T const*
    {
        return std::addressof(t);
    }

    constexpr auto operator()(T const* const t) const noexcept -> T const*
    {
        return t;
    }

    constexpr auto operator()(std::reference_wrapper<T> const& t) const noexcept
            -> T const*
    {
        return std::addressof(t.get());
    }

    constexpr auto
    operator()(std::reference_wrapper<T const> const& t) const noexcept
            -> T const*
    {
        return std::addressof(t.get());
    }
};

} // namespace detail

template <class T, class Compare>
struct address_compare
{
    template <class U, class V>
    constexpr auto operator()(U const& l, V const& r) const noexcept -> bool
    {
        return Compare{}(
                detail::unwrap_const_addressof<T>{}(l),
                detail::unwrap_const_addressof<T>{}(r));
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
