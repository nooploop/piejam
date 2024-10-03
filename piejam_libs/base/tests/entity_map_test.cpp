// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/entity_map.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

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
    auto id = sut.lock().emplace(5);
    EXPECT_NE(entity_map<int>::id_t{}, id);
    EXPECT_FALSE(sut.empty());
    EXPECT_EQ(1, sut.size());
    EXPECT_TRUE(sut.contains(id));
}

TEST(entity_map, remove)
{
    entity_map<int> sut;
    auto id = sut.lock().emplace(5);
    ASSERT_FALSE(sut.empty());

    sut.lock().erase(id);
    EXPECT_TRUE(sut.empty());
}

TEST(entity_map, remove_remove)
{
    entity_map<int> sut;
    auto id = sut.lock().emplace(5);
    ASSERT_FALSE(sut.empty());

    EXPECT_EQ(1u, sut.lock().erase(id));
    EXPECT_TRUE(sut.empty());

    EXPECT_EQ(0, sut.lock().erase(id));
}

TEST(entity_map, add_add_add)
{
    entity_map<int> sut;
    auto id1 = sut.lock().emplace(2);
    auto id2 = sut.lock().emplace(3);
    auto id3 = sut.lock().emplace(5);
    EXPECT_THAT(
            sut,
            testing::ElementsAre(
                    std::make_pair(id1, 2),
                    std::make_pair(id2, 3),
                    std::make_pair(id3, 5)));
}

} // namespace piejam::test
