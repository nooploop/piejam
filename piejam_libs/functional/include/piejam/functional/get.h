// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/hof/returns.hpp>

namespace piejam
{

template <std::size_t I>
inline constexpr auto get_by_index =
        [](auto&& x) BOOST_HOF_RETURNS(get<I>(std::forward<decltype(x)>(x)));

template <class T>
inline constexpr auto get_by_type =
        [](auto&& x) BOOST_HOF_RETURNS(get<T>(std::forward<decltype(x)>(x)));

} // namespace piejam
