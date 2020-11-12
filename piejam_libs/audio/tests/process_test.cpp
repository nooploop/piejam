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

#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/process.h>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>

namespace piejam::audio::engine::test
{

namespace
{

struct test_dummy_dag_executor : public dag_executor
{
    void operator()(std::size_t) {}
};

} // namespace

TEST(process_test, swap_executor)
{
    std::atomic_bool running{true};
    process sut(2, 2);
    std::thread process_thread([&running, &sut] {
        range::table_view<float const> in_audio;
        range::table_view<float> out_audio;
        while (running.load(std::memory_order_relaxed))
        {
            sut(in_audio, out_audio);
        }
    });

    for (std::size_t i = 0; i < 10000; ++i)
        sut.swap_executor(std::make_unique<test_dummy_dag_executor>());

    running = false;
    process_thread.join();
    SUCCEED();
}

} // namespace piejam::audio::engine::test
