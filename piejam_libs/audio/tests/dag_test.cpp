// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/dag.h>

#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/thread_context.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(dag, add_task_and_run)
{
    int x{};
    dag sut;

    sut.add_task([&x](auto const&, std::size_t) { x = 1; });
    (*sut.make_runnable())(1);

    EXPECT_EQ(1, x);
}

TEST(dag, add_child_task_and_run)
{
    int x{};
    dag sut;

    auto parent_id = sut.add_task([&x](auto const&, std::size_t) { x = 5; });
    sut.add_child_task(parent_id, [&x](auto const&, std::size_t) { x -= 3; });
    (*sut.make_runnable())(1);

    EXPECT_EQ(2, x);
}

TEST(dag, add_child_and_run)
{
    int x{};
    dag sut;

    auto parent_id = sut.add_task([&x](auto const&, std::size_t) { x = 5; });
    auto child_id = sut.add_task([&x](auto const&, std::size_t) { x += 3; });
    sut.add_child(parent_id, child_id);
    (*sut.make_runnable())(1);

    EXPECT_EQ(8, x);
}

TEST(dag, split_and_merge_graph)
{
    int x{}, y{}, z{};
    dag sut;

    auto parent_id = sut.add_task([&x](auto const&, std::size_t) { x = 5; });
    auto child1_id =
            sut.add_child_task(parent_id, [&y](auto const&, std::size_t) {
                y = 2;
            });
    auto child2_id =
            sut.add_child_task(parent_id, [&z](auto const&, std::size_t) {
                z = 3;
            });
    auto result_id = sut.add_child_task(
            child1_id,
            [&x, &y, &z](auto const&, std::size_t) { x += y + z; });
    sut.add_child(child2_id, result_id);
    (*sut.make_runnable())(1);

    EXPECT_EQ(10, x);
}

} // namespace piejam::audio::engine::test
