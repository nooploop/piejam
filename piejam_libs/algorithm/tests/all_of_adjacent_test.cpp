// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/all_of_adjacent.h>

#include <gtest/gtest.h>

#include <vector>

namespace piejam::algorithm::test
{

TEST(all_of_adjacent, empty)
{
    EXPECT_TRUE(all_of_adjacent(std::vector<int>(), std::less<>{}));
}

TEST(all_of_adjacent, with_one_element)
{
    EXPECT_TRUE(all_of_adjacent(std::vector<int>({1}), std::less<>{}));
}

TEST(all_of_adjacent, true_for_all)
{
    EXPECT_TRUE(
            all_of_adjacent(std::vector<int>({1, 2, 3, 4, 5}), std::less<>{}));
}

TEST(all_of_adjacent, false_for_some)
{
    EXPECT_FALSE(
            all_of_adjacent(std::vector<int>({1, 2, 3, 5, 5}), std::less<>{}));
    EXPECT_FALSE(
            all_of_adjacent(std::vector<int>({1, 2, 3, 5, 4}), std::less<>{}));
    EXPECT_FALSE(
            all_of_adjacent(std::vector<int>({1, 2, 4, 3, 5}), std::less<>{}));
    EXPECT_FALSE(
            all_of_adjacent(std::vector<int>({2, 1, 3, 4, 5}), std::less<>{}));
}

} // namespace piejam::algorithm::test
