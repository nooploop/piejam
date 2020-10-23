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

#include <piejam/audio/engine/dag.h>

#include <piejam/audio/engine/thread_context.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(dag, add_task_and_run)
{
    int x{};
    dag sut(20);

    sut.add_task([&x](auto const&) { x = 1; });
    sut({});

    EXPECT_EQ(1, x);
}

TEST(dag, add_child_task_and_run)
{
    int x{};
    dag sut(20);

    auto parent_id = sut.add_task([&x](auto const&) { x = 5; });
    sut.add_child_task(parent_id, [&x](auto const&) { x -= 3; });
    sut({});

    EXPECT_EQ(2, x);
}

TEST(dag, add_child_and_run)
{
    int x{};
    dag sut(20);

    auto parent_id = sut.add_task([&x](auto const&) { x = 5; });
    auto child_id = sut.add_task([&x](auto const&) { x += 3; });
    sut.add_child(parent_id, child_id);
    sut({});

    EXPECT_EQ(8, x);
}

TEST(dag, split_and_merge_graph)
{
    int x{}, y{}, z{};
    dag sut(20);

    auto parent_id = sut.add_task([&x](auto const&) { x = 5; });
    auto child1_id =
            sut.add_child_task(parent_id, [&y](auto const&) { y = 2; });
    auto child2_id =
            sut.add_child_task(parent_id, [&z](auto const&) { z = 3; });
    auto result_id = sut.add_child_task(child1_id, [&x, &y, &z](auto const&) {
        x += y + z;
    });
    sut.add_child(child2_id, result_id);
    sut({});

    EXPECT_EQ(10, x);
}

} // namespace piejam::audio::engine::test
