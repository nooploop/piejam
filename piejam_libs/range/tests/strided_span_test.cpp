// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/range/strided_span.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>

namespace piejam::range::test
{

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
