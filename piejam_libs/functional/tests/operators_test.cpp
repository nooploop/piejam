// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/functional/operators.h>

#include <gtest/gtest.h>

namespace piejam::test
{

TEST(partial_compare, equal_to)
{
    EXPECT_TRUE(equal_to<>(5)(5));
    EXPECT_FALSE(equal_to<>(5)(6));

    EXPECT_TRUE(equal_to<>(5, 5));
    EXPECT_FALSE(equal_to<>(5, 6));
}

TEST(partial_compare, not_equal_to)
{
    EXPECT_TRUE(not_equal_to<>(5)(6));
    EXPECT_FALSE(not_equal_to<>(5)(5));

    EXPECT_TRUE(not_equal_to<>(5, 6));
    EXPECT_FALSE(not_equal_to<>(5, 5));
}

TEST(partial_compare, less)
{
    EXPECT_FALSE(less<>(5)(6));
    EXPECT_FALSE(less<>(5)(5));
    EXPECT_TRUE(less<>(5)(4));

    EXPECT_FALSE(less<>(6, 5));
    EXPECT_FALSE(less<>(5, 5));
    EXPECT_TRUE(less<>(4, 5));
}

TEST(partial_compare, less_equal)
{
    EXPECT_FALSE(less<>(5)(6));
    EXPECT_TRUE(less_equal<>(5)(5));
    EXPECT_TRUE(less_equal<>(5)(4));

    EXPECT_FALSE(less<>(6, 5));
    EXPECT_TRUE(less_equal<>(5, 5));
    EXPECT_TRUE(less_equal<>(4, 5));
}

TEST(partial_compare, greater)
{
    EXPECT_FALSE(greater<>(5)(4));
    EXPECT_FALSE(greater<>(5)(5));
    EXPECT_TRUE(greater<>(5)(6));

    EXPECT_FALSE(greater<>(4, 5));
    EXPECT_FALSE(greater<>(5, 5));
    EXPECT_TRUE(greater<>(6, 5));
}

TEST(partial_compare, greater_equal)
{
    EXPECT_FALSE(greater_equal<>(5)(4));
    EXPECT_TRUE(greater_equal<>(5)(5));
    EXPECT_TRUE(greater_equal<>(5)(6));

    EXPECT_FALSE(greater_equal<>(4, 5));
    EXPECT_TRUE(greater_equal<>(5, 5));
    EXPECT_TRUE(greater_equal<>(6, 5));
}

TEST(indirection_op, static_asserts)
{
    int* a{};
    int* const b{};
    int const* c{};
    int const* const d{};
    static_assert(std::is_same_v<int&, decltype(indirection_op(a))>);
    static_assert(std::is_same_v<int&, decltype(indirection_op(b))>);
    static_assert(std::is_same_v<int const&, decltype(indirection_op(c))>);
    static_assert(std::is_same_v<int const&, decltype(indirection_op(d))>);

    using p_t = int*;
    static_assert(std::is_same_v<int&, decltype(indirection_op(p_t{}))>);
}

TEST(indirection_op, derefs_a_pointer)
{
    int x{5};
    int* const b = &x;
    int const* const d = &x;

    EXPECT_EQ(5, indirection_op(b));
    EXPECT_EQ(5, indirection_op(d));
}

} // namespace piejam::test
