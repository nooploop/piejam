// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/dag.h>

#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/process.h>
#include <piejam/range/table_view.h>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>

namespace piejam::audio::engine::test
{

namespace
{

struct test_dummy_dag_executor : public dag_executor
{
    void operator()(std::size_t) override
    {
    }
};

} // namespace

TEST(process_test, swap_executor)
{
    std::atomic_bool running{true};
    process sut;
    std::thread process_thread([&running, &sut] {
        while (running.load(std::memory_order_relaxed))
        {
            sut(2);
        }
    });

    for (std::size_t i = 0; i < 10000; ++i)
    {
        EXPECT_TRUE(
                sut.swap_executor(std::make_unique<test_dummy_dag_executor>()));
    }

    running = false;
    process_thread.join();
}

TEST(process_test, swap_executor_fails_if_processing_thread_is_not_running)
{
    process sut;
    EXPECT_FALSE(
            sut.swap_executor(std::make_unique<test_dummy_dag_executor>()));
}

} // namespace piejam::audio::engine::test
