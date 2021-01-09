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
    auto second =
            first != std::end(rng) ? std::next(std::begin(rng)) : std::end(rng);
    auto const end = std::end(rng);

    while (second != end)
        std::invoke(std::forward<BinaryOp>(op), *first++, *second++);
}

} // namespace piejam::algorithm
