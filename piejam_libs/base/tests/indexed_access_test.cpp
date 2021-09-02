// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/indexed_access.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <vector>

namespace piejam::test
{

TEST(insert_at, into_empty)
{
    std::vector<int> vec;
    insert_at(vec, 0, 5);

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(5))(vec));
}

TEST(insert_at, with_one_element_at_begin)
{
    std::vector<int> vec({5});
    insert_at(vec, 0, 2);

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(2, 5))(vec));
}

TEST(insert_at, with_one_element_at_end)
{
    std::vector<int> vec({5});
    insert_at(vec, 1, 2);

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(5, 2))(vec));
}

TEST(insert_at, multiple_elements)
{
    std::vector<int> vec({5});
    using testing::ElementsAre;
    using testing::Matches;

    insert_at(vec, 0, 2);
    EXPECT_TRUE(Matches(ElementsAre(2, 5))(vec));

    insert_at(vec, 1, 4);
    EXPECT_TRUE(Matches(ElementsAre(2, 4, 5))(vec));

    insert_at(vec, 1, 3);
    EXPECT_TRUE(Matches(ElementsAre(2, 3, 4, 5))(vec));

    insert_at(vec, 4, 7);
    EXPECT_TRUE(Matches(ElementsAre(2, 3, 4, 5, 7))(vec));

    insert_at(vec, 4, 6);
    EXPECT_TRUE(Matches(ElementsAre(2, 3, 4, 5, 6, 7))(vec));

    insert_at(vec, 6, 8);
    EXPECT_TRUE(Matches(ElementsAre(2, 3, 4, 5, 6, 7, 8))(vec));
}

TEST(erase_at, remove_single)
{
    std::vector<int> vec({5});
    erase_at(vec, 0);
    EXPECT_TRUE(vec.empty());
}

TEST(erase_at, remove_first)
{
    std::vector<int> vec({1, 2, 3, 4, 5});
    erase_at(vec, 0);

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(2, 3, 4, 5))(vec));
}

TEST(erase_at, remove_middle)
{
    std::vector<int> vec({1, 2, 3, 4, 5});
    erase_at(vec, 2);

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(1, 2, 4, 5))(vec));
}

TEST(erase_at, remove_last)
{
    std::vector<int> vec({1, 2, 3, 4, 5});
    erase_at(vec, 4);

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(1, 2, 3, 4))(vec));
}

} // namespace piejam::test
