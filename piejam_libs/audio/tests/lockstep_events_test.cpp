// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/lockstep_events.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

namespace piejam::audio::engine::test
{

struct lockstep_events_test : ::testing::Test
{
    event_buffer_memory ev_buf_mem{1u << 16};
    std::pmr::memory_resource* ev_mem{&ev_buf_mem.memory_resource()};
};

TEST_F(lockstep_events_test, single_empty_buffer)
{
    event_buffer<float> buf(ev_mem);
    lockstep_events([](std::size_t, float) {}, std::tuple(0.f), buf);
}

TEST_F(lockstep_events_test, single_buffer_with_one_event)
{
    event_buffer<float> buf(ev_mem);
    buf.insert(3, 1.f);
    float value{};
    std::tie(value) =
            lockstep_events([](std::size_t, float) {}, std::tuple(value), buf);
    EXPECT_FLOAT_EQ(1.f, value);
}

TEST_F(lockstep_events_test, single_buffer_with_two_consecutive_events)
{
    event_buffer<float> buf(ev_mem);
    buf.insert(3, 1.f);
    buf.insert(5, 3.f);
    float value{};
    std::tie(value) =
            lockstep_events([](std::size_t, float) {}, std::tuple(value), buf);
    EXPECT_FLOAT_EQ(3.f, value);
}

TEST_F(lockstep_events_test, two_buffer_with_multiple_events)
{
    event_buffer<int> buf1(ev_mem);
    event_buffer<int> buf2(ev_mem);
    buf1.insert(3, 1);
    buf1.insert(5, 2);
    buf1.insert(7, 3);
    buf1.insert(7, 4);
    buf2.insert(2, 1);
    buf2.insert(4, 2);
    buf2.insert(6, 3);
    buf2.insert(7, 4);
    std::vector<std::pair<std::size_t, int>> result;
    int value1{};
    int value2{};
    std::tie(value1, value2) = lockstep_events(
            [&result](std::size_t offset, int v1, int v2) {
                result.emplace_back(offset, v1 * v2);
            },
            std::tuple(value1, value2),
            buf1,
            buf2);

    std::vector<std::pair<std::size_t, int>>
            expected{{2, 0}, {3, 1}, {4, 2}, {5, 4}, {6, 6}, {7, 12}, {7, 16}};
    EXPECT_EQ(4, value1);
    EXPECT_EQ(4, value2);
    EXPECT_TRUE(std::ranges::equal(expected, result));
}

} // namespace piejam::audio::engine::test
