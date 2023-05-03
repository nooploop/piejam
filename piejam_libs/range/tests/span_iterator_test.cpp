// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/span_iterator.h>

#include <gtest/gtest.h>

namespace piejam::range::test
{

TEST(span_iterator, static_assert_test)
{
    static_assert(std::random_access_iterator<span_iterator<float>>);
    static_assert(std::random_access_iterator<span_iterator<float, 0>>);
    static_assert(std::random_access_iterator<span_iterator<float, 1>>);
    static_assert(std::random_access_iterator<span_iterator<float, 2>>);
    static_assert(
            std::is_nothrow_default_constructible_v<span_iterator<float>>);
}

TEST(span_iterator, non_const_to_const)
{
    std::array<float, 2> a{1.f, -1.f};
    span_iterator<float, 2> nc_it(a);
    span_iterator<float const, 2> it = nc_it;

    EXPECT_EQ(1.f, (*it)[0]);
    EXPECT_EQ(-1.f, (*it)[1]);
}

TEST(span_iterator, static_to_runtime)
{
    std::array<float, 2> a{1.f, -1.f};
    span_iterator<float, 2> nc_it(a);
    span_iterator<float> it = nc_it;

    EXPECT_EQ(1.f, (*it)[0]);
    EXPECT_EQ(-1.f, (*it)[1]);
}

TEST(span_iterator, static_non_const_to_runtime_const)
{
    std::array<float, 2> a{1.f, -1.f};
    span_iterator<float, 2> nc_it(a);
    span_iterator<float const> it = nc_it;

    EXPECT_EQ(1.f, (*it)[0]);
    EXPECT_EQ(-1.f, (*it)[1]);
}

TEST(span_iterator, equality)
{
    std::array<float, 2> a{1.f, -1.f};
    span_iterator<float, 2> it1(a);
    span_iterator<float, 2> it2(a);

    EXPECT_EQ(it1, it2);
    EXPECT_FALSE(it1 != it2);
}

} // namespace piejam::range::test
