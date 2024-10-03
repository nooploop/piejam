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
[[nodiscard]]
constexpr auto
all_of_adjacent(It first, It last, BinaryPredicate&& p) -> bool
{
    return last == std::ranges::adjacent_find(
                           std::move(first),
                           last,
                           std::not_fn(std::forward<BinaryPredicate>(p)));
}

template <
        std::ranges::forward_range Range,
        range_binary_predicate<Range> BinaryPredicate>
[[nodiscard]]
constexpr auto
all_of_adjacent(Range const& rng, BinaryPredicate&& p) -> bool
{
    return all_of_adjacent(
            std::ranges::begin(rng),
            std::ranges::end(rng),
            std::forward<BinaryPredicate>(p));
}

} // namespace piejam::algorithm
