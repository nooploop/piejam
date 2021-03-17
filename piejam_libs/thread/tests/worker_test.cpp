// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/worker.h>

#include <gtest/gtest.h>

namespace piejam::thread::test
{

TEST(worker, executes_after_wakeup)
{
    std::atomic_bool worked{false};

    worker wt;
    wt.wakeup([&]() { worked.store(true, std::memory_order_release); });

    wt.wakeup([]() {}); // block until we finished with the previous task

    EXPECT_TRUE(worked);
}

TEST(worker, on_multiple_wakeups_block_until_previous_task_is_finished)
{
    std::size_t counter1{};
    std::size_t counter2{};
    bool select{};

    worker wt;
    for (std::size_t i = 0; i < 100; ++i)
    {
        if (select)
            wt.wakeup([&]() { ++counter1; });
        else
            wt.wakeup([&]() { ++counter2; });
        select = !select;
    }

    wt.wakeup([]() {}); // block until we finished previous tasks

    EXPECT_EQ(50u, counter1);
    EXPECT_EQ(50u, counter2);
}

} // namespace piejam::thread::test
