// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/hof/combine.hpp>
#include <boost/hof/identity.hpp>
#include <boost/hof/partial.hpp>

#include <functional>

namespace piejam::tuple
{

template <std::size_t I>
struct element_compare
{
    template <class T, class Compare>
    static inline constexpr auto
            compare = boost::hof::partial(boost::hof::combine(
                    Compare{},
                    boost::hof::identity,
                    [](auto&& t) -> decltype(auto) {
                        return std::get<I>(std::forward<decltype(t)>(t));
                    }));

    template <class T>
    static constexpr decltype(auto) equal_to(T&& t)
    {
        return compare<T, std::equal_to<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) not_equal_to(T&& t)
    {
        return compare<T, std::not_equal_to<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) less(T&& t)
    {
        return compare<T, std::less<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) less_equal(T&& t)
    {
        return compare<T, std::less_equal<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) greater(T&& t)
    {
        return compare<T, std::greater<>>(std::forward<T>(t));
    }

    template <class T>
    static constexpr decltype(auto) greater_equal(T&& t)
    {
        return compare<T, std::greater_equal<>>(std::forward<T>(t));
    }
};

template <std::size_t I>
inline constexpr auto element = element_compare<I>{};

} // namespace piejam::tuple
