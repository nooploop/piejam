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

#include <piejam/meta/decay_tuple_elements.h>

#include <gtest/gtest.h>

namespace piejam::meta::test
{

TEST(decay_tuple_elements, verify)
{
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_tuple_elements_t<std::tuple<int, int>>>);
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_tuple_elements_t<std::tuple<int const&, int>>>);
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_tuple_elements_t<std::tuple<int&, int>>>);
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_tuple_elements_t<std::tuple<int&&, int>>>);
    static_assert(
            std::is_same_v<std::tuple<>, decay_tuple_elements_t<std::tuple<>>>);
}

} // namespace piejam::meta::test
