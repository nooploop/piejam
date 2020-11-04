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

#include <piejam/thread/worker.h>

#include <gtest/gtest.h>

namespace piejam::thread::test
{

TEST(worker, doesnt_execute_if_not_woken_up)
{
    std::atomic_bool worked{false};

    worker wt([&]() { worked.store(true, std::memory_order_release); });
    std::this_thread::sleep_for(std::chrono::microseconds{100});

    EXPECT_FALSE(worked);
}

TEST(worker, executes_after_wakeup)
{
    std::atomic_bool worked{false};

    worker wt([&]() { worked.store(true, std::memory_order_release); });
    wt.wakeup();
    std::this_thread::sleep_for(std::chrono::microseconds{100});

    EXPECT_TRUE(worked);
}

} // namespace piejam::thread::test
