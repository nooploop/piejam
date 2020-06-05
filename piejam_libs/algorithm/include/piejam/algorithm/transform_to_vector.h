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

template <class Range, class F>
auto
transform_to_vector(Range const& rng, F&& f)
{
    std::vector<std::decay_t<decltype(f(*std::begin(rng)))>> result;
    std::transform(
            std::begin(rng),
            std::end(rng),
            std::back_inserter(result),
            std::forward<F>(f));
    return result;
}

} // namespace piejam::algorithm
