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

    template <class Value>
    static constexpr decltype(auto) equal_to(Value&& v)
    {
        return compare<std::equal_to<>>(std::forward<Value>(v));
    }

    template <class Value>
    static constexpr decltype(auto) not_equal_to(Value&& v)
    {
        return compare<std::not_equal_to<>>(std::forward<Value>(v));
    }

    template <class Value>
    static constexpr decltype(auto) less(Value&& v)
    {
        return compare<std::less<>>(std::forward<Value>(v));
    }

    template <class Value>
    static constexpr decltype(auto) less_equal(Value&& v)
    {
        return compare<std::less_equal<>>(std::forward<Value>(v));
    }

    template <class Value>
    static constexpr decltype(auto) greater(Value&& v)
    {
        return compare<std::greater<>>(std::forward<Value>(v));
    }

    template <class Value>
    static constexpr decltype(auto) greater_equal(Value&& v)
    {
        return compare<std::greater_equal<>>(std::forward<Value>(v));
    }
};

template <std::size_t I>
inline constexpr auto element = element_compare<I>{};

} // namespace piejam::tuple
