// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/container/table.h>

#include <boost/align/is_aligned.hpp>

#include <gtest/gtest.h>

namespace piejam::container::test
{

TEST(table, data_is_aligned_with_std_allocator)
{
    table<int> sut(3, 3);

    EXPECT_TRUE(boost::alignment::is_aligned(
            sut.rows()[0].data(),
            allocator_alignment_v<std::allocator<float>>));
    EXPECT_TRUE(boost::alignment::is_aligned(
            sut.rows()[1].data(),
            allocator_alignment_v<std::allocator<float>>));
    EXPECT_TRUE(boost::alignment::is_aligned(
            sut.rows()[2].data(),
            allocator_alignment_v<std::allocator<float>>));
}

TEST(table, set_and_get)
{
    table<int> sut(3, 3);
    sut.rows()[0][2] = 2;
    sut.rows()[1][1] = 3;
    sut.rows()[2][0] = 5;

    EXPECT_EQ(2, sut.rows()[0][2]);
    EXPECT_EQ(3, sut.rows()[1][1]);
    EXPECT_EQ(5, sut.rows()[2][0]);
}

TEST(table, set_and_get_from_rows_to_columns)
{
    table<int> sut(3, 3);
    sut.rows()[0][2] = 2;
    sut.rows()[1][1] = 3;
    sut.rows()[2][0] = 5;

    EXPECT_EQ(2, sut.columns()[2][0]);
    EXPECT_EQ(3, sut.columns()[1][1]);
    EXPECT_EQ(5, sut.columns()[0][2]);
}

} // namespace piejam::container::test
