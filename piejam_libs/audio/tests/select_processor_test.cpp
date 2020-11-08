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

#include <piejam/audio/engine/select_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/functional/partial_compare.h>

#include <gtest/gtest.h>

#include <algorithm>

namespace piejam::audio::engine::test
{

struct select_processor_test : ::testing::Test
{
    audio_slice in1{.23f};
    audio_slice in2{.58f};
    audio_slice in3{.77f};
    std::vector<std::reference_wrapper<audio_slice const>> inputs{
            in1,
            in2,
            in3};
    std::array<float, 16> out_buf{};
    std::vector<std::span<float>> outputs{out_buf};
    std::vector<audio_slice> results{outputs[0]};
    event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_mem{&ev_buf_mem.memory_resource()};
    event_buffer<std::size_t> index_event_buffer{ev_mem};
    event_input_buffers ev_in_bufs;
    event_output_buffers ev_out_bufs;
    process_context ctx{inputs, outputs, results, ev_in_bufs, ev_out_bufs, 16};
    std::unique_ptr<processor> sut{make_select_processor(3)};

    select_processor_test()
    {
        ev_in_bufs.add(event_port(std::in_place_type<std::size_t>));
        ev_in_bufs.set(0, index_event_buffer);
    }
};

TEST_F(select_processor_test, default_constructed_selects_first_input)
{
    sut->process(ctx);
    ASSERT_TRUE(results[0].is_constant());
    EXPECT_FLOAT_EQ(.23f, results[0].constant());
}

TEST_F(select_processor_test, select_second_input_at_begin)
{
    index_event_buffer.insert(0, 1);

    sut->process(ctx);
    ASSERT_TRUE(results[0].is_constant());
    EXPECT_FLOAT_EQ(.58f, results[0].constant());
}

TEST_F(select_processor_test, select_second_input_in_the_middle)
{
    index_event_buffer.insert(8, 1);

    sut->process(ctx);
    ASSERT_TRUE(results[0].is_buffer());

    std::array<float, 16> expected;
    std::fill_n(std::fill_n(expected.begin(), 8, .23f), 8, .58f);

    EXPECT_TRUE(std::ranges::equal(results[0].buffer(), expected));
}

TEST_F(select_processor_test, select_second_then_third_input)
{
    index_event_buffer.insert(0, 1);
    index_event_buffer.insert(8, 2);

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_buffer());

    std::array<float, 16> expected;
    std::fill_n(std::fill_n(expected.begin(), 8, .58f), 8, .77f);

    EXPECT_TRUE(std::ranges::equal(results[0].buffer(), expected));
}

TEST_F(select_processor_test,
       select_out_of_bounds_at_begin_will_result_in_silence)
{
    index_event_buffer.insert(0, 10);

    sut->process(ctx);
    ASSERT_TRUE(results[0].is_constant());
    EXPECT_FLOAT_EQ(0.f, results[0].constant());
}

} // namespace piejam::audio::engine::test
