// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/contains.h>

#include <gtest/gtest.h>

#include <array>

namespace piejam::algorithm::test
{

TEST(contains, returns_true_if_found)
{
    EXPECT_TRUE(contains(std::array{2, 3, 5, 7, 11, 13}, 11));
}

TEST(contains, returns_false_if_not_found)
{
    EXPECT_FALSE(contains(std::array{2, 3, 5, 7, 11, 13}, 12));
}

TEST(contains_if, returns_true_if_found)
{
    EXPECT_TRUE(contains_if(std::array{2, 3, 5, 7, 11, 13}, [](int x) {
        return x == 5;
    }));
}

TEST(contains_if, returns_false_if_not_found)
{
    EXPECT_FALSE(contains_if(std::array{2, 3, 5, 7, 11, 13}, [](int x) {
        return x > 23;
    }));
}

} // namespace piejam::algorithm::test
