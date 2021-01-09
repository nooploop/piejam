// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
