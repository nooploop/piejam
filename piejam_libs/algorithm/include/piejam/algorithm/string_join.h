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
#include <string>

namespace piejam::algorithm
{

template <class Range>
auto
string_join(Range const& rng, char sep) -> std::string
{
    if (!std::empty(rng))
    {
        auto first = std::begin(rng);
        std::string acc = *first;
        std::for_each(
                std::next(first),
                std::end(rng),
                [&acc, sep](std::string const& s) { (acc += sep) += s; });
        return acc;
    }
    else
        return std::string();
}

} // namespace piejam::algorithm
