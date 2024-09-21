// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/pan_balance_processor.h>

#include <piejam/audio/engine/event_buffer.h>
#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/audio/pan.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

struct pan_balance_processor_test : public testing::TestWithParam<float>
{
    pan_balance_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
        ev_out_bufs.add(event_port(std::in_place_type<float>, "left gain"));
        ev_out_bufs.add(event_port(std::in_place_type<float>, "right gain"));

        ev_in_bufs.add(event_port(std::in_place_type<float>, "pan_balance"));
        ev_in_bufs.set(0, ev_in_buf);
    }

    audio::engine::event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_buf_pmr_mem{&ev_buf_mem.memory_resource()};
    audio::engine::event_buffer<float> ev_in_buf{ev_buf_pmr_mem};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    static constexpr std::size_t const buffer_size{16};
    audio::engine::process_context ctx{
            .outputs = {},
            .results = {},
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = buffer_size};
};

TEST_P(pan_balance_processor_test, pan)
{
    auto sut = make_pan_processor();

    ev_in_buf.insert(3, GetParam());

    sut->process(ctx);

    auto const& left = ev_out_bufs.get<float>(0);
    auto const& right = ev_out_bufs.get<float>(1);

    auto const expected = sinusoidal_constant_power_pan(GetParam());

    ASSERT_EQ(1u, left.size());
    EXPECT_EQ(3u, left.begin()->offset());
    EXPECT_FLOAT_EQ(expected.left, left.begin()->value());

    ASSERT_EQ(1u, right.size());
    EXPECT_EQ(3u, right.begin()->offset());
    EXPECT_FLOAT_EQ(expected.right, right.begin()->value());
}

TEST_P(pan_balance_processor_test, balance)
{
    auto sut = make_balance_processor();

    ev_in_buf.insert(3, GetParam());

    sut->process(ctx);

    auto const& left = ev_out_bufs.get<float>(0);
    auto const& right = ev_out_bufs.get<float>(1);

    auto const expected = stereo_balance(GetParam());

    ASSERT_EQ(1u, left.size());
    EXPECT_EQ(3u, left.begin()->offset());
    EXPECT_FLOAT_EQ(expected.left, left.begin()->value());

    ASSERT_EQ(1u, right.size());
    EXPECT_EQ(3u, right.begin()->offset());
    EXPECT_FLOAT_EQ(expected.right, right.begin()->value());
}

INSTANTIATE_TEST_SUITE_P(
        verify,
        pan_balance_processor_test,
        testing::Values(0.f, -1.f, -0.5f, 1.f, 0.5f));

TEST_F(pan_balance_processor_test, pan_empty_input)
{
    auto sut = make_pan_processor();

    sut->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<float>(0).empty());
    EXPECT_TRUE(ev_out_bufs.get<float>(1).empty());
}

TEST_F(pan_balance_processor_test, stereo_balance_empty_input)
{
    auto sut = make_pan_processor();

    sut->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<float>(0).empty());
    EXPECT_TRUE(ev_out_bufs.get<float>(1).empty());
}

} // namespace piejam::audio::engine::test
