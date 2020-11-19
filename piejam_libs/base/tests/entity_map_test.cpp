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

#include <piejam/entity_map.h>

#include <gtest/gtest.h>

#include <algorithm>

namespace piejam::test
{

TEST(entity_map, default_ctor)
{
    entity_map<int> sut;
    EXPECT_TRUE(sut.empty());
    EXPECT_EQ(0, sut.size());
}

TEST(entity_map, add)
{
    entity_map<int> sut;
    auto id = sut.add(5);
    EXPECT_NE(entity_map<int>::id_t{}, id);
    EXPECT_FALSE(sut.empty());
    EXPECT_EQ(1, sut.size());
    EXPECT_TRUE(sut.contains(id));
}

TEST(entity_map, remove)
{
    entity_map<int> sut;
    auto id = sut.add(5);
    ASSERT_FALSE(sut.empty());

    sut.remove(id);
    EXPECT_TRUE(sut.empty());
}

TEST(entity_map, remove_remove)
{
    entity_map<int> sut;
    auto id = sut.add(5);
    ASSERT_FALSE(sut.empty());

    EXPECT_EQ(1u, sut.remove(id));
    EXPECT_TRUE(sut.empty());

    EXPECT_EQ(0, sut.remove(id));
}

TEST(entity_map, add_add_add)
{
    entity_map<int> sut;
    sut.add(2);
    sut.add(3);
    sut.add(5);
    EXPECT_TRUE(std::ranges::equal(
            sut,
            std::initializer_list<int>{2, 3, 5},
            std::equal_to<>{},
            &entity_map<int>::map_t::value_type::second));
}

} // namespace piejam::test
