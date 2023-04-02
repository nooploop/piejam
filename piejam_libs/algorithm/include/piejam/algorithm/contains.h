// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <std::ranges::input_range Range, convertible_to_range_value<Range> T>
constexpr auto
contains(Range const& rng, T&& value) -> bool
{
    using std::end;

    return std::ranges::find(rng, std::forward<T>(value)) != end(rng);
}

template <
        std::ranges::input_range Range,
        range_unary_predicate<Range> Predicate>
constexpr auto
contains_if(Range const& rng, Predicate&& p) -> bool
{
    using std::end;

    return std::ranges::find_if(rng, std::forward<Predicate>(p)) != end(rng);
}

} // namespace piejam::algorithm
