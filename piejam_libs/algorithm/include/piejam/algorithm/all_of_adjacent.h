// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <
        std::input_iterator It,
        std::predicate<std::iter_value_t<It>, std::iter_value_t<It>>
                BinaryPredicate>
constexpr bool
all_of_adjacent(It&& first, It&& last, BinaryPredicate&& p)
{
    return std::forward<It>(last) ==
           std::adjacent_find(
                   std::forward<It>(first),
                   std::forward<It>(last),
                   std::not_fn(std::forward<BinaryPredicate>(p)));
}

template <
        std::ranges::input_range Range,
        std::predicate<
                std::ranges::range_value_t<Range>,
                std::ranges::range_value_t<Range>> BinaryPredicate>
constexpr bool
all_of_adjacent(Range&& rng, BinaryPredicate&& p)
{
    return all_of_adjacent(
            std::begin(std::forward<Range>(rng)),
            std::end(std::forward<Range>(rng)),
            std::forward<BinaryPredicate>(p));
}

} // namespace piejam::algorithm
