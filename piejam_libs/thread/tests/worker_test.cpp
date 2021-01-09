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
    worker::task_t task(
            [&]() { worked.store(true, std::memory_order_release); });
    wt.wakeup(task);
    std::this_thread::sleep_for(std::chrono::microseconds{100});

    EXPECT_TRUE(worked);
}

} // namespace piejam::thread::test
