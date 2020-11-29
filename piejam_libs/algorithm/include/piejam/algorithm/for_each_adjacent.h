// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
