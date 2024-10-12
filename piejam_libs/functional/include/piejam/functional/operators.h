// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/hof/returns.hpp>

#include <functional>

namespace piejam
{

namespace detail
{

template <class Operator, class Proj>
struct op
{
    template <class X>
    constexpr auto operator()(X&& x, Proj proj = {}) const noexcept
    {
        return [proj_x = proj(std::forward<X>(x)),
                proj = std::move(proj)]<class Y>(Y&& y) noexcept {
            return Operator{}(proj(std::forward<Y>(y)), proj_x);
        };
    }

    template <class X, class Y>
    constexpr auto operator()(X&& x, Y&& y, Proj proj = {}) const noexcept
    {
        return Operator{}(proj(std::forward<X>(x)), proj(std::forward<Y>(y)));
    }
};

} // namespace detail

template <class Proj = std::identity>
inline constexpr auto equal_to = detail::op<std::equal_to<>, Proj>{};

template <class Proj = std::identity>
inline constexpr auto not_equal_to = detail::op<std::not_equal_to<>, Proj>{};

template <class Proj = std::identity>
inline constexpr auto less = detail::op<std::less<>, Proj>{};

template <class Proj = std::identity>
inline constexpr auto less_equal = detail::op<std::less_equal<>, Proj>{};

template <class Proj = std::identity>
inline constexpr auto greater = detail::op<std::greater<>, Proj>{};

template <class Proj = std::identity>
inline constexpr auto greater_equal = detail::op<std::greater_equal<>, Proj>{};

template <class Proj = std::identity>
inline constexpr auto multiplies = detail::op<std::multiplies<>, Proj>{};

inline constexpr auto indirection_op =
        []<class V>(V&& v) BOOST_HOF_RETURNS(*std::forward<V>(v));

inline constexpr auto addressof_op =
        []<class V>(V&& v) BOOST_HOF_RETURNS(&std::forward<V>(v));

template <class IndexType, IndexType N>
inline constexpr auto index_op =
        []<class V>(V&& v) BOOST_HOF_RETURNS(std::forward<V>(v)[N]);

} // namespace piejam
