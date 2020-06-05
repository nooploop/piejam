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

#include <algorithm>
#include <iterator>

namespace piejam::algorithm
{

template <class Range1, class Range2, class OutputIt>
auto
set_intersection(Range1&& rng1, Range2&& rng2, OutputIt&& out) -> OutputIt
{
    return std::set_intersection(
            std::begin(rng1),
            std::end(rng1),
            std::begin(rng2),
            std::end(rng2),
            std::forward<OutputIt>(out));
}

} // namespace piejam::algorithm
