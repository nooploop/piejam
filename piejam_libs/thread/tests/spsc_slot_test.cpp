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

#include <piejam/thread/spsc_slot.h>

#include <gtest/gtest.h>

namespace piejam::thread::test
{

TEST(spsc_slot, pull)
{
    spsc_slot<int> sut;

    int r{58};
    EXPECT_FALSE(sut.pull(r));
    EXPECT_EQ(58, r);
}

TEST(spsc_slot, push_pull)
{
    spsc_slot<int> sut;
    sut.push(23);

    int r{};
    EXPECT_TRUE(sut.pull(r));
    EXPECT_EQ(23, r);
}

TEST(spsc_slot, push_push_pull)
{
    spsc_slot<int> sut;
    sut.push(23);
    sut.push(58);

    int r{};
    EXPECT_TRUE(sut.pull(r));
    EXPECT_EQ(58, r);
}

} // namespace piejam::thread::test
