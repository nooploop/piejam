// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
