// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/strided_span.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <ranges>

namespace piejam::range::test
{

TEST(strided_span, static_asserts)
{
    static_assert(std::ranges::random_access_range<strided_span<float>>);
    static_assert(std::ranges::viewable_range<strided_span<float> const&>);
    static_assert(std::ranges::sized_range<strided_span<float>>);
}

TEST(strided_span, default_ctor)
{
    strided_span<int> sut;

    EXPECT_EQ(nullptr, sut.data());
    EXPECT_EQ(0u, sut.size());
    EXPECT_EQ(0, sut.stride());
    EXPECT_TRUE(sut.empty());
    EXPECT_TRUE(sut.begin() == sut.end());
    EXPECT_FALSE(sut.begin() != sut.end());
    EXPECT_FALSE(sut.begin() < sut.end());
    EXPECT_FALSE(sut.end() < sut.begin());
    EXPECT_TRUE(sut.begin() <= sut.end());
    EXPECT_TRUE(sut.end() <= sut.begin());
    EXPECT_TRUE(sut.rbegin() == sut.rend());
    EXPECT_FALSE(sut.rbegin() != sut.rend());
}

TEST(strided_span, with_zero_size)
{
    std::array<int, 2> arr{1, 2};

    strided_span<int> sut(arr.data(), 0, 1);

    EXPECT_EQ(arr.data(), sut.data());
    EXPECT_EQ(0u, sut.size());
    EXPECT_EQ(1, sut.stride());
    EXPECT_TRUE(sut.empty());
    EXPECT_TRUE(sut.begin() == sut.end());
    EXPECT_FALSE(sut.begin() != sut.end());
    EXPECT_FALSE(sut.begin() < sut.end());
    EXPECT_FALSE(sut.end() < sut.begin());
    EXPECT_TRUE(sut.begin() <= sut.end());
    EXPECT_TRUE(sut.end() <= sut.begin());
    EXPECT_TRUE(sut.rbegin() == sut.rend());
    EXPECT_FALSE(sut.rbegin() != sut.rend());
}

TEST(strided_span, with_some_data_size_stride)
{
    std::array<int, 9> arr{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::array<int, 4> expected{1, 3, 5, 7};
    std::array<int, 4> reverse_expected{7, 5, 3, 1};

    strided_span<int> sut(arr.data(), 4u, 2);
    EXPECT_EQ(arr.data(), sut.data());
    EXPECT_EQ(4u, sut.size());
    EXPECT_EQ(2, sut.stride());
    EXPECT_FALSE(sut.empty());
    EXPECT_FALSE(sut.begin() == sut.end());
    EXPECT_TRUE(sut.begin() != sut.end());
    EXPECT_TRUE(sut.begin() < sut.end());
    EXPECT_FALSE(sut.end() < sut.begin());
    EXPECT_TRUE(sut.begin() <= sut.end());
    EXPECT_FALSE(sut.end() <= sut.begin());
    EXPECT_FALSE(sut.rbegin() == sut.rend());
    EXPECT_TRUE(sut.rbegin() != sut.rend());
    EXPECT_TRUE(std::equal(sut.begin(), sut.end(), expected.begin()));
    EXPECT_TRUE(std::equal(sut.rbegin(), sut.rend(), reverse_expected.begin()));
    EXPECT_EQ(1, sut[0]);
    EXPECT_EQ(3, sut[1]);
    EXPECT_EQ(5, sut[2]);
    EXPECT_EQ(7, sut[3]);
}

} // namespace piejam::range::test
