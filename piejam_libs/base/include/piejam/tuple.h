// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/mp11/algorithm.hpp>

namespace piejam::tuple
{

template <class Tuple>
using decay_elements_t = boost::mp11::mp_transform<std::decay_t, Tuple>;

namespace detail
{

template <class Tp, std::size_t... J, class F>
constexpr auto
for_each_while_impl(Tp&& tp, std::integer_sequence<std::size_t, J...>, F&& f)
        -> bool
{
    return (f(get<J>(std::forward<Tp>(tp))) && ...);
}

template <class Tp, class F>
constexpr auto
for_each_while_impl(Tp&& /*tp*/, std::integer_sequence<std::size_t>, F&& /*f*/)
        -> bool
{
    return true;
}

} // namespace detail

template <class Tp, class F>
auto
for_each_while(Tp&& tp, F&& f) -> bool
{
    using seq = std::make_index_sequence<
            std::tuple_size<typename std::remove_reference<Tp>::type>::value>;
    return detail::for_each_while_impl(
            std::forward<Tp>(tp),
            seq{},
            std::forward<F>(f));
}

namespace detail
{

template <class Tp, std::size_t... J, class F>
constexpr auto
for_each_until_impl(Tp&& tp, std::integer_sequence<std::size_t, J...>, F&& f)
        -> bool
{
    return (f(get<J>(std::forward<Tp>(tp))) || ...);
}

template <class Tp, class F>
constexpr auto
for_each_until_impl(Tp&& /*tp*/, std::integer_sequence<std::size_t>, F&& /*f*/)
        -> bool
{
    return false;
}

} // namespace detail

template <class Tp, class F>
auto
for_each_until(Tp&& tp, F&& f) -> bool
{
    using seq = std::make_index_sequence<
            std::tuple_size<typename std::remove_reference<Tp>::type>::value>;
    return detail::for_each_until_impl(
            std::forward<Tp>(tp),
            seq{},
            std::forward<F>(f));
}

} // namespace piejam::tuple
