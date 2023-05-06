// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/entity_map.h>

#include <gmock/gmock-matchers.h>
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
    auto id1 = sut.add(2);
    auto id2 = sut.add(3);
    auto id3 = sut.add(5);
    EXPECT_THAT(
            sut,
            testing::ElementsAre(
                    std::make_pair(id1, 2),
                    std::make_pair(id2, 3),
                    std::make_pair(id3, 5)));
}

} // namespace piejam::test
