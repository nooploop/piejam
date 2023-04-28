// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/hof/returns.hpp>

#include <variant>

namespace piejam
{

template <std::size_t I>
inline constexpr auto get_if_by_index = [](auto&& x)
        BOOST_HOF_RETURNS(std::get_if<I>(std::forward<decltype(x)>(x)));

template <class T>
inline constexpr auto get_if_by_type = [](auto&& x)
        BOOST_HOF_RETURNS(std::get_if<T>(std::forward<decltype(x)>(x)));

} // namespace piejam
