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

#include <piejam/range/table_view.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>

namespace piejam::range::test
{

TEST(row_major_table_view, iterate_and_compare)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> sut(arr.data(), 2, 3, 4, 1);

    auto row1 = *sut.begin();
    std::array expected_row1{1, 2, 3};
    EXPECT_TRUE(std::equal(row1.begin(), row1.end(), expected_row1.begin()));

    auto row2 = *std::next(sut.begin());
    std::array expected_row2{4, 5, 6};
    EXPECT_TRUE(std::equal(row2.begin(), row2.end(), expected_row2.begin()));
}

TEST(row_major_table_view, transpose)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> tv(arr.data(), 2, 3, 4, 1);

    auto sut = transpose(tv);

    auto col1 = *sut.begin();
    std::array expected_col1{1, 4};
    EXPECT_TRUE(std::equal(col1.begin(), col1.end(), expected_col1.begin()));

    auto col2 = *std::next(sut.begin());
    std::array expected_col2{2, 5};
    EXPECT_TRUE(std::equal(col2.begin(), col2.end(), expected_col2.begin()));

    auto col3 = *std::next(sut.begin(), 2);
    std::array expected_col3{3, 6};
    EXPECT_TRUE(std::equal(col3.begin(), col3.end(), expected_col3.begin()));
}

TEST(row_major_table_view, stepped_columns)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 0, 2, 0, 3, 4, 0, 5, 0, 6};

    table_view<int> sut(arr.data(), 2, 3, 5, 2);

    auto row1 = *sut.begin();
    std::array expected_row1{1, 2, 3};
    EXPECT_TRUE(std::equal(row1.begin(), row1.end(), expected_row1.begin()));

    auto row2 = *std::next(sut.begin());
    std::array expected_row2{4, 5, 6};
    EXPECT_TRUE(std::equal(row2.begin(), row2.end(), expected_row2.begin()));
}

TEST(row_major_table_view, stepped_columns_transpose)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 0, 2, 0, 3, 4, 0, 5, 0, 6};

    table_view<int> tv(arr.data(), 2, 3, 5, 2);

    auto sut = transpose(tv);

    auto col1 = *sut.begin();
    std::array expected_col1{1, 4};
    EXPECT_TRUE(std::equal(col1.begin(), col1.end(), expected_col1.begin()));

    auto col2 = *std::next(sut.begin());
    std::array expected_col2{2, 5};
    EXPECT_TRUE(std::equal(col2.begin(), col2.end(), expected_col2.begin()));

    auto col3 = *std::next(sut.begin(), 2);
    std::array expected_col3{3, 6};
    EXPECT_TRUE(std::equal(col3.begin(), col3.end(), expected_col3.begin()));
}

TEST(column_major_table_view, iterate_and_compare)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> sut(arr.data(), 3, 2, 1, 4);

    auto col1 = *sut.begin();
    std::array expected_col1{1, 4};
    EXPECT_TRUE(std::equal(col1.begin(), col1.end(), expected_col1.begin()));

    auto col2 = *std::next(sut.begin());
    std::array expected_col2{2, 5};
    EXPECT_TRUE(std::equal(col2.begin(), col2.end(), expected_col2.begin()));

    auto col3 = *std::next(sut.begin(), 2);
    std::array expected_col3{3, 6};
    EXPECT_TRUE(std::equal(col3.begin(), col3.end(), expected_col3.begin()));
}

TEST(column_major_table_view, transpose)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> tv(arr.data(), 3, 2, 1, 4);

    auto sut = transpose(tv);

    auto row1 = *sut.begin();
    std::array expected_row1{1, 2, 3};
    EXPECT_TRUE(std::equal(row1.begin(), row1.end(), expected_row1.begin()));

    auto row2 = *std::next(sut.begin());
    std::array expected_row2{4, 5, 6};
    EXPECT_TRUE(std::equal(row2.begin(), row2.end(), expected_row2.begin()));
}

TEST(table_view, fill)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> sut(arr.data(), 2, 3, 4, 1);

    ASSERT_TRUE(std::equal(
            sut[0].begin(),
            sut[0].end(),
            std::array{1, 2, 3}.begin()));
    ASSERT_TRUE(std::equal(
            sut[1].begin(),
            sut[1].end(),
            std::array{4, 5, 6}.begin()));

    fill(sut, 23);

    EXPECT_TRUE(std::equal(
            sut[0].begin(),
            sut[0].end(),
            std::array{23, 23, 23}.begin()));
    EXPECT_TRUE(std::equal(
            sut[1].begin(),
            sut[1].end(),
            std::array{23, 23, 23}.begin()));
    EXPECT_TRUE(std::equal(
            arr.begin(),
            arr.end(),
            std::array{23, 23, 23, 0, 23, 23, 23, 0}.begin()));
}

TEST(table_view_as_const, data_stays_same)
{
    std::array arr{1.f, 2.f, 3.f, 4.f};
    table_view<float> tv(arr.data(), 2, 2, 1, 1);
    auto ctv = as_const(tv);
    EXPECT_EQ(tv.data(), ctv.data());
    EXPECT_EQ(tv.major_size(), ctv.major_size());
    EXPECT_EQ(tv.major_step(), ctv.major_step());
    EXPECT_EQ(tv.minor_size(), ctv.minor_size());
    EXPECT_EQ(tv.minor_step(), ctv.minor_step());
}

TEST(table_view_as_const, types_converts_to_const)
{
    table_view<float> tv;
    auto ctv = as_const(tv);
    static_assert(std::is_same_v<table_view<float const>, decltype(ctv)>);

    auto ctv2 = as_const(tv);
    static_assert(std::is_same_v<table_view<float const>, decltype(ctv2)>);
}

} // namespace piejam::range::test
