// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>

namespace piejam
{

namespace detail
{

template <class Compare>
struct compare
{
    template <class X>
    constexpr auto operator()(X&& x) const noexcept
    {
        return [x](auto&& y) noexcept {
            return Compare{}(std::forward<decltype(y)>(y), x);
        };
    }

    template <class X, class Y>
    constexpr auto operator()(X&& x, Y&& y) const noexcept
    {
        return Compare{}(std::forward<X>(x), std::forward<Y>(y));
    }
};

} // namespace detail

inline constexpr auto equal_to = detail::compare<std::equal_to<>>{};
inline constexpr auto not_equal_to = detail::compare<std::not_equal_to<>>{};
inline constexpr auto less = detail::compare<std::less<>>{};
inline constexpr auto less_equal = detail::compare<std::less_equal<>>{};
inline constexpr auto greater = detail::compare<std::greater<>>{};
inline constexpr auto greater_equal = detail::compare<std::greater_equal<>>{};

} // namespace piejam
