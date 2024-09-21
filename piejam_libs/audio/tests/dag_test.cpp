// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/dag.h>

#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/thread_context.h>
#include <piejam/thread/worker.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(dag, add_task_and_run)
{
    int x{};
    dag sut;

    sut.add_task([&x](auto const&) { x = 1; });
    (*sut.make_runnable())(1);

    EXPECT_EQ(1, x);
}

TEST(dag, add_child_task_and_run)
{
    int x{};
    dag sut;

    auto parent_id = sut.add_task([&x](auto const&) { x = 5; });
    sut.add_child_task(parent_id, [&x](auto const&) { x -= 3; });
    (*sut.make_runnable())(1);

    EXPECT_EQ(2, x);
}

TEST(dag, add_child_and_run)
{
    int x{};
    dag sut;

    auto parent_id = sut.add_task([&x](auto const&) { x = 5; });
    auto child_id = sut.add_task([&x](auto const&) { x += 3; });
    sut.add_child(parent_id, child_id);
    (*sut.make_runnable())(1);

    EXPECT_EQ(8, x);
}

TEST(dag, split_and_merge_graph)
{
    int x{}, y{}, z{};
    dag sut;

    auto parent_id = sut.add_task([&x](auto const&) { x = 5; });
    auto child1_id =
            sut.add_child_task(parent_id, [&y](auto const&) { y = 2; });
    auto child2_id =
            sut.add_child_task(parent_id, [&z](auto const&) { z = 3; });
    auto result_id = sut.add_child_task(child1_id, [&x, &y, &z](auto const&) {
        x += y + z;
    });
    sut.add_child(child2_id, result_id);
    (*sut.make_runnable())(1);

    EXPECT_EQ(10, x);
}

TEST(dag, split_and_merge_graph_mt)
{
    int x{}, y{}, z{};
    dag sut;

    auto parent_id = sut.add_task([&x](auto const&) { x = 5; });
    auto child1_id =
            sut.add_child_task(parent_id, [&y](auto const&) { y = 2; });
    auto child2_id =
            sut.add_child_task(parent_id, [&z](auto const&) { z = 3; });
    auto result_id = sut.add_child_task(child1_id, [&x, &y, &z](auto const&) {
        x += y + z;
    });
    sut.add_child(child2_id, result_id);

    std::unique_ptr<audio::engine::dag_executor> executor;
    {
        std::vector<thread::worker> workers(2);
        executor = sut.make_runnable(workers);
        for (std::size_t n = 0; n < 10; ++n)
        {
            (*executor)(1);
            EXPECT_EQ(10, x);
        }
    }
}

} // namespace piejam::audio::engine::test
