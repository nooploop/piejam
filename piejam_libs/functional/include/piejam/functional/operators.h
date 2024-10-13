// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/hof/partial.hpp>

#include <functional>

namespace piejam
{

inline constexpr auto equal_to = boost::hof::partial(std::equal_to<>{});

inline constexpr auto not_equal_to = boost::hof::partial(std::not_equal_to<>{});

inline constexpr auto less = boost::hof::partial(std::less<>{});

inline constexpr auto less_equal = boost::hof::partial(std::less_equal<>{});

inline constexpr auto greater = boost::hof::partial(std::greater<>{});

inline constexpr auto greater_equal =
        boost::hof::partial(std::greater_equal<>{});

inline constexpr auto multiplies = boost::hof::partial(std::multiplies<>{});

} // namespace piejam
