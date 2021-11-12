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

template <std::input_iterator It, class BinaryPredicate>
constexpr bool
all_of_adjacent(It&& first, It&& last, BinaryPredicate&& p)
{
    return std::forward<It>(last) ==
           std::adjacent_find(
                   std::forward<It>(first),
                   std::forward<It>(last),
                   std::not_fn(std::forward<BinaryPredicate>(p)));
}

template <std::ranges::input_range Range, class BinaryPredicate>
constexpr bool
all_of_adjacent(Range&& rng, BinaryPredicate&& p)
{
    using std::begin;
    using std::end;

    return all_of_adjacent(
            begin(std::forward<Range>(rng)),
            end(std::forward<Range>(rng)),
            std::forward<BinaryPredicate>(p));
}

} // namespace piejam::algorithm
