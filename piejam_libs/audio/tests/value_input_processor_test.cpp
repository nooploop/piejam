// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/value_io_processor.h>

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

struct value_input_processor_test : ::testing::Test
{
    std::size_t const buffer_size{16};
    event_buffer_memory ev_buf_mem{1024};
    event_input_buffers ev_in_bufs;
    event_output_buffers ev_out_bufs;

    value_input_processor_test()
    {
        ev_in_bufs.add(event_port(std::in_place_type<float>));
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
    }
};

TEST_F(value_input_processor_test,
       initial_process_will_send_an_initial_event_with_initial_value)
{
    value_io_processor<float> sut(0.23f);
    ev_out_bufs.add(sut.event_outputs()[0]);

    sut.process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_buf = ev_out_bufs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());

    auto const& ev = *ev_out_buf.begin();
    EXPECT_EQ(0, ev.offset());
    EXPECT_FLOAT_EQ(.23f, ev.value());
}

TEST_F(value_input_processor_test,
       no_event_in_subsequent_process_if_input_value_doesnt_change)
{
    value_io_processor<float> sut(0.23f);
    ev_out_bufs.add(sut.event_outputs()[0]);

    sut.process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_buf = ev_out_bufs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    ev_out_bufs.clear_buffers();
    ASSERT_EQ(0, ev_out_buf.size());

    sut.process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});
    EXPECT_EQ(0, ev_out_buf.size());
}

TEST_F(value_input_processor_test, change_event_if_subsequent_value_changes)
{
    value_io_processor<float> sut(0.23f);
    ev_out_bufs.add(sut.event_outputs()[0]);

    sut.process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_buf = ev_out_bufs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    ev_out_bufs.clear_buffers();
    ASSERT_EQ(0, ev_out_buf.size());

    sut.set(.58f);
    sut.process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});
    ASSERT_EQ(1, ev_out_buf.size());

    auto const& ev = *ev_out_buf.begin();
    EXPECT_EQ(0, ev.offset());
    EXPECT_FLOAT_EQ(.58f, ev.value());
}

} // namespace piejam::audio::engine::test
