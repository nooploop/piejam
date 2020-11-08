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

#include <piejam/audio/components/gui_input_processor.h>

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

namespace piejam::audio::components::test
{

struct gui_input_processor_test : ::testing::Test
{
    std::size_t const buffer_size{16};
    engine::event_buffer_memory ev_buf_mem{1024};
    engine::event_output_buffers ev_out_bufs;

    gui_input_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
    }
};

TEST_F(gui_input_processor_test,
       initial_process_will_send_an_initial_event_with_initial_value)
{
    components::gui_input_processor<float> sut(0.23f);
    ev_out_bufs.add(sut.event_outputs()[0]);

    sut.process({{}, {}, {}, {}, ev_out_bufs, buffer_size});

    auto const& ev_out_buf = ev_out_bufs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());

    auto const& ev = *ev_out_buf.begin();
    EXPECT_EQ(0, ev.offset());
    EXPECT_FLOAT_EQ(.23f, ev.value());
}

TEST_F(gui_input_processor_test,
       no_event_in_subsequent_process_if_input_value_doesnt_change)
{
    components::gui_input_processor<float> sut(0.23f);
    ev_out_bufs.add(sut.event_outputs()[0]);

    sut.process({{}, {}, {}, {}, ev_out_bufs, buffer_size});

    auto const& ev_out_buf = ev_out_bufs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    ev_out_bufs.clear_buffers();
    ASSERT_EQ(0, ev_out_buf.size());

    sut.process({{}, {}, {}, {}, ev_out_bufs, buffer_size});
    EXPECT_EQ(0, ev_out_buf.size());
}

TEST_F(gui_input_processor_test, change_event_if_subsequent_value_changes)
{
    components::gui_input_processor<float> sut(0.23f);
    ev_out_bufs.add(sut.event_outputs()[0]);

    sut.process({{}, {}, {}, {}, ev_out_bufs, buffer_size});

    auto const& ev_out_buf = ev_out_bufs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    ev_out_bufs.clear_buffers();
    ASSERT_EQ(0, ev_out_buf.size());

    sut.set(.58f);
    sut.process({{}, {}, {}, {}, ev_out_bufs, buffer_size});
    ASSERT_EQ(1, ev_out_buf.size());

    auto const& ev = *ev_out_buf.begin();
    EXPECT_EQ(0, ev.offset());
    EXPECT_FLOAT_EQ(.58f, ev.value());
}

} // namespace piejam::audio::components::test
