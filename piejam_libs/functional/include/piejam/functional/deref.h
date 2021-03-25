// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/hof/returns.hpp>

namespace piejam
{

inline constexpr auto deref =
        [](auto&& v) BOOST_HOF_RETURNS(*std::forward<decltype(v)>(v));

} // namespace piejam
