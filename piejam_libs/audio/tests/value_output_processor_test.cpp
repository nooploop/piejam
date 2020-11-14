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
