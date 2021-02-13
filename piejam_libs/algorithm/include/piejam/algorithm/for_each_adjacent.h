// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>
#include <iterator>

namespace piejam::algorithm
{

template <class Range, class BinaryOp>
void
for_each_adjacent(Range&& rng, BinaryOp&& op)
{
    auto first = std::begin(rng);
    auto const last = std::end(rng);
    auto second = first != last ? std::next(std::begin(rng)) : last;

    while (second != last)
        std::invoke(std::forward<BinaryOp>(op), *first++, *second++);
}

} // namespace piejam::algorithm
