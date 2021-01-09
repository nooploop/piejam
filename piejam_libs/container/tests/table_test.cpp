// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/container/table.h>

#include <boost/align/aligned_allocator.hpp>

#include <gtest/gtest.h>

namespace piejam::container::test
{

TEST(table, data_is_aligned_with_std_allocator)
{
    table<int> sut(2, 3);
    sut.rows()[1][1] = 1;

    EXPECT_EQ(1, sut.rows()[1][1]);
}

} // namespace piejam::container::test
