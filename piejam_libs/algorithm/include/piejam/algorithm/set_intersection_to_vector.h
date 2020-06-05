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
#include <type_traits>
#include <vector>

namespace piejam::algorithm
{

template <class Range1, class Range2>
auto
set_intersection_to_vector(Range1 const& r1, Range2 const& r2)
{
    using r1_value_type = std::decay_t<decltype(*std::begin(r1))>;
    using r2_value_type = std::decay_t<decltype(*std::begin(r2))>;
    std::vector<std::common_type_t<r1_value_type, r2_value_type>> result;
    std::set_intersection(
            std::begin(r1),
            std::end(r1),
            std::begin(r2),
            std::end(r2),
            std::back_inserter(result));
    return result;
}

} // namespace piejam::algorithm
