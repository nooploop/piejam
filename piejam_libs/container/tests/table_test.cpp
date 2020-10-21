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
