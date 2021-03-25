// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/functional/compare.h>

#include <gtest/gtest.h>

namespace piejam::test
{

TEST(partial_compare, equal_to)
{
    EXPECT_TRUE(equal_to(5)(5));
    EXPECT_FALSE(equal_to(5)(6));

    EXPECT_TRUE(equal_to(5, 5));
    EXPECT_FALSE(equal_to(5, 6));
}

TEST(partial_compare, not_equal_to)
{
    EXPECT_TRUE(not_equal_to(5)(6));
    EXPECT_FALSE(not_equal_to(5)(5));

    EXPECT_TRUE(not_equal_to(5, 6));
    EXPECT_FALSE(not_equal_to(5, 5));
}

TEST(partial_compare, less)
{
    EXPECT_FALSE(less(5)(6));
    EXPECT_FALSE(less(5)(5));
    EXPECT_TRUE(less(5)(4));

    EXPECT_FALSE(less(6, 5));
    EXPECT_FALSE(less(5, 5));
    EXPECT_TRUE(less(4, 5));
}

TEST(partial_compare, less_equal)
{
    EXPECT_FALSE(less(5)(6));
    EXPECT_TRUE(less_equal(5)(5));
    EXPECT_TRUE(less_equal(5)(4));

    EXPECT_FALSE(less(6, 5));
    EXPECT_TRUE(less_equal(5, 5));
    EXPECT_TRUE(less_equal(4, 5));
}

TEST(partial_compare, greater)
{
    EXPECT_FALSE(greater(5)(4));
    EXPECT_FALSE(greater(5)(5));
    EXPECT_TRUE(greater(5)(6));

    EXPECT_FALSE(greater(4, 5));
    EXPECT_FALSE(greater(5, 5));
    EXPECT_TRUE(greater(6, 5));
}

TEST(partial_compare, greater_equal)
{
    EXPECT_FALSE(greater_equal(5)(4));
    EXPECT_TRUE(greater_equal(5)(5));
    EXPECT_TRUE(greater_equal(5)(6));

    EXPECT_FALSE(greater_equal(4, 5));
    EXPECT_TRUE(greater_equal(5, 5));
    EXPECT_TRUE(greater_equal(6, 5));
}

} // namespace piejam::test
