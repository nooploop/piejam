// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <functional>
#include <iterator>

namespace piejam::algorithm
{

template <std::ranges::forward_range Range, range_binary_op<Range> BinaryOp>
constexpr void
for_each_adjacent(Range&& rng, BinaryOp&& op)
{
    using std::begin;
    using std::end;

    auto first = begin(rng);
    auto const last = end(rng);
    auto second = first != last ? std::next(first) : last;

    while (second != last)
    {
        std::invoke(std::forward<BinaryOp>(op), *first++, *second++);
    }
}

} // namespace piejam::algorithm
