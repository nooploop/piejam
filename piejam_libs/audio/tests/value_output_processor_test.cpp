// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/value_output_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

struct value_output_processor_test : ::testing::Test
{
    std::size_t const buffer_size{16};
    std::pmr::memory_resource* ev_mem{std::pmr::get_default_resource()};
    event_buffer<int> ev_buf{ev_mem};
    event_input_buffers ev_in_bufs;
    event_output_buffers ev_out_bufs;
    value_output_processor<int> sut;
    process_context ctx{{}, {}, {}, ev_in_bufs, ev_out_bufs, 128};

    value_output_processor_test()
    {
        ev_in_bufs.add(event_port(std::in_place_type<int>));
        ev_in_bufs.set(0, ev_buf);
    }

    ~value_output_processor_test() { ev_buf.clear(); }
};

TEST_F(value_output_processor_test, no_value_initially)
{
    sut.consume([](int) { FAIL(); });
}

TEST_F(value_output_processor_test, no_value_after_process_with_no_events)
{
    sut.process(ctx);
    sut.consume([](int) { FAIL(); });
}

TEST_F(value_output_processor_test, value_on_event)
{
    ev_buf.insert(0, 23);
    sut.process(ctx);

    int result{};
    EXPECT_TRUE(sut.get(result));
    EXPECT_EQ(23, result);
}

} // namespace piejam::audio::engine::test
