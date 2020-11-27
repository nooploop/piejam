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

#include <iterator>

namespace piejam
{

template <class Container, class T>
auto
insert_at(Container&& c, std::size_t const index, T&& value)
{
    return std::forward<Container>(c).insert(
            std::next(std::begin(std::forward<Container>(c)), index),
            std::forward<T>(value));
}

template <class Container>
auto
erase_at(Container&& c, std::size_t const index)
{
    return std::forward<Container>(c).erase(
            std::next(std::begin(std::forward<Container>(c)), index));
}

} // namespace piejam
