// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>

namespace piejam
{

namespace detail
{

template <class Operator>
struct op
{
    template <class X>
    constexpr auto operator()(X&& x) const noexcept
    {
        return [x = std::forward<X>(x)]<class Y>(Y&& y) noexcept {
            return Operator{}(std::forward<Y>(y), x);
        };
    }

    template <class X, class Y>
    constexpr auto operator()(X&& x, Y&& y) const noexcept
    {
        return Operator{}(std::forward<X>(x), std::forward<Y>(y));
    }
};

} // namespace detail

inline constexpr auto equal_to = detail::op<std::equal_to<>>{};

inline constexpr auto not_equal_to = detail::op<std::not_equal_to<>>{};

inline constexpr auto less = detail::op<std::less<>>{};

inline constexpr auto less_equal = detail::op<std::less_equal<>>{};

inline constexpr auto greater = detail::op<std::greater<>>{};

inline constexpr auto greater_equal = detail::op<std::greater_equal<>>{};

inline constexpr auto multiplies = detail::op<std::multiplies<>>{};

} // namespace piejam
