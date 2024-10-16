// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/mp11/algorithm.hpp>

#include <functional>
#include <type_traits>

namespace piejam::tuple
{

template <class Tuple>
using decay_elements_t = boost::mp11::mp_transform<std::decay_t, Tuple>;

template <class Tp, class F>
auto
for_each_while(Tp&& tp, F&& f) -> bool
{
    return []<std::size_t... I>(
                   [[maybe_unused]] Tp&& tp,
                   [[maybe_unused]]
                   F f,
                   std::index_sequence<I...>) {
        return (true && ... &&
                std::invoke(std::forward<F>(f), get<I>(std::forward<Tp>(tp))));
    }(std::forward<Tp>(tp),
           std::forward<F>(f),
           std::make_index_sequence<
                   std::tuple_size_v<std::remove_reference_t<Tp>>>{});
}

template <class Tp, class F>
auto
for_each_until(Tp&& tp, F&& f) -> bool
{
    return []<std::size_t... I>(
                   [[maybe_unused]] Tp&& tp,
                   [[maybe_unused]]
                   F f,
                   std::index_sequence<I...>) {
        return (false || ... ||
                std::invoke(std::forward<F>(f), get<I>(std::forward<Tp>(tp))));
    }(std::forward<Tp>(tp),
           std::forward<F>(f),
           std::make_index_sequence<
                   std::tuple_size_v<std::remove_reference_t<Tp>>>{});
}

} // namespace piejam::tuple
