// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/job_deque.h>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>

namespace piejam::thread::test
{

TEST(job_deque, pop_from_empty_will_return_null)
{
    job_deque<int, 128> sut;
    EXPECT_EQ(nullptr, sut.pop());
}

TEST(job_deque, push_pop_and_then_pop_will_return_null)
{
    job_deque<int, 128> sut;
    int j;
    sut.push(&j);
    EXPECT_EQ(&j, sut.pop());
    EXPECT_EQ(nullptr, sut.pop());
}

TEST(job_deque, steal_from_empty_will_return_null)
{
    job_deque<int, 128> sut;
    EXPECT_EQ(nullptr, sut.steal());
}

TEST(job_deque, push_steal_will_return_pushed)
{
    job_deque<int, 128> sut;
    int j;
    sut.push(&j);
    EXPECT_EQ(&j, sut.steal());
}

TEST(job_deque, push_steal_and_then_pop_will_return_null)
{
    job_deque<int, 128> sut;
    int j;
    sut.push(&j);
    EXPECT_EQ(&j, sut.steal());
    EXPECT_EQ(nullptr, sut.pop());
}

TEST(job_deque, push_pop_and_steal_concurrently)
{
    job_deque<int, 128> sut;
    int j;
    std::atomic_bool finished{false};

    constexpr std::size_t num_runs{10000};
    constexpr std::size_t num_pushes{128};
    std::size_t num_pops{};
    std::size_t num_steals{};

    auto steal_thread = std::thread([&sut, &finished, &j, &num_steals] {
        while (!finished.load(std::memory_order_relaxed))
        {
            auto const r = sut.steal();
            if (r != nullptr)
            {
                EXPECT_EQ(r, &j);
                ++num_steals;
            }
        }
    });

    auto gen_thread = std::thread([&sut, &finished, &j, &num_pops]() {
        for (std::size_t n = 0; n < num_runs; ++n)
        {
            for (std::size_t i = 0; i < num_pushes; ++i)
            {
                sut.push(&j);
            }

            for (std::size_t i = 0; i < num_pushes; ++i)
            {
                auto const r = sut.pop();
                if (r != nullptr)
                {
                    EXPECT_EQ(r, &j);
                    ++num_pops;
                }
            }
        }

        finished.store(true, std::memory_order_release);
    });

    steal_thread.join();
    gen_thread.join();

    EXPECT_EQ(num_runs * num_pushes, num_pops + num_steals);
}

} // namespace piejam::thread::test
