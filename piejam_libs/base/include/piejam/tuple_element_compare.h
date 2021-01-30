// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>

namespace piejam::tuple
{

template <std::size_t I>
struct element_compare
{
    template <class Compare, class Value>
    static constexpr decltype(auto) compare(Value&& v)
    {
        return [v](auto&& tup) {
            return Compare{}(v, std::get<I>(std::forward<decltype(tup)>(tup)));
        };
    }

    template <class T>
    static constexpr decltype(auto) equal_to(T&& t)
    {
        return compare<std::equal_to<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) not_equal_to(T&& t)
    {
        return compare<std::not_equal_to<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) less(T&& t)
    {
        return compare<std::less<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) less_equal(T&& t)
    {
        return compare<std::less_equal<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) greater(T&& t)
    {
        return compare<std::greater<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) greater_equal(T&& t)
    {
        return compare<std::greater_equal<>>(std::forward<T>(t));
    }
};

template <std::size_t I>
inline constexpr auto element = element_compare<I>{};

} // namespace piejam::tuple
