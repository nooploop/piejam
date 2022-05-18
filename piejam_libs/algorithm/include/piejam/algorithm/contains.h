// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <
        std::ranges::input_range Range,
        std::convertible_to<std::ranges::range_value_t<Range>> T>
constexpr auto
contains(Range const& rng, T&& value) -> bool
{
    using std::end;

    return std::ranges::find(rng, std::forward<T>(value)) != end(rng);
}

template <
        std::ranges::input_range Range,
        std::indirect_unary_predicate<std::ranges::iterator_t<Range>> Predicate>
constexpr auto
contains_if(Range const& rng, Predicate&& p) -> bool
{
    using std::end;

    return std::ranges::find_if(rng, std::forward<Predicate>(p)) != end(rng);
}

} // namespace piejam::algorithm
