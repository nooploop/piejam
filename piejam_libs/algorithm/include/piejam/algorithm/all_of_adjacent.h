// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <
        std::forward_iterator It,
        std::indirect_binary_predicate<It, It> BinaryPredicate>
[[nodiscard]] constexpr auto
all_of_adjacent(It&& first, It&& last, BinaryPredicate&& p) -> bool
{
    return std::forward<It>(last) ==
           std::ranges::adjacent_find(
                   std::forward<It>(first),
                   std::forward<It>(last),
                   std::not_fn(std::forward<BinaryPredicate>(p)));
}

template <
        std::ranges::forward_range Range,
        range_binary_predicate<Range> BinaryPredicate>
[[nodiscard]] constexpr auto
all_of_adjacent(Range&& rng, BinaryPredicate&& p) -> bool
{
    using std::begin;
    using std::end;

    return all_of_adjacent(
            begin(std::forward<Range>(rng)),
            end(std::forward<Range>(rng)),
            std::forward<BinaryPredicate>(p));
}

} // namespace piejam::algorithm
