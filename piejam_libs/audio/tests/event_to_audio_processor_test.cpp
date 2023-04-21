// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/smoother_processor.h>

#include <piejam/algorithm/all_of_adjacent.h>
#include <piejam/audio/engine/event_buffer.h>
#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/functional/compare.h>

#include <gtest/gtest.h>

#include <algorithm>

namespace piejam::audio::engine::test
{

struct event_to_audio_processor_test : testing::Test
{
    event_to_audio_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
        ev_in_bufs.add(event_port(std::in_place_type<float>));
        ev_in_bufs.set(0, ev_in_buf);
    }

    audio::engine::event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_buf_pmr_mem{&ev_buf_mem.memory_resource()};
    audio::engine::event_buffer<float> ev_in_buf{ev_buf_pmr_mem};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    static constexpr std::size_t const buffer_size{
            default_smooth_length * 2};
    std::array<float, buffer_size> out0;
    std::array<std::span<float>, 1> outputs{out0};
    std::array<audio_slice, 1> results;
    audio::engine::process_context ctx{
            .outputs = outputs,
            .results = results,
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = buffer_size};
};

TEST_F(event_to_audio_processor_test, num_io)
{
    auto sut = make_smoother_processor();
    EXPECT_EQ(0u, sut->num_inputs());
    EXPECT_EQ(1u, sut->num_outputs());
    EXPECT_EQ(1u, sut->event_inputs().size());
    EXPECT_EQ(0u, sut->event_outputs().size());
}

TEST_F(event_to_audio_processor_test, default_will_produce_silence)
{
    auto sut = make_smoother_processor();
    sut->process(ctx);

    ASSERT_TRUE(ctx.results[0].is_constant());
    EXPECT_FLOAT_EQ(0.f, ctx.results[0].constant());
}

TEST_F(event_to_audio_processor_test, event_at_begin_of_buffer)
{
    auto sut = make_smoother_processor();

    ev_in_buf.insert(0u, 1.f);

    sut->process(ctx);

    ASSERT_TRUE(ctx.results[0].is_buffer());
    EXPECT_EQ(out0.data(), ctx.results[0].buffer().data());

    ev_in_buf.clear();

    sut->process(ctx);

    // after first process, the smoother should be finished and generate a
    // constant
    ASSERT_TRUE(ctx.results[0].is_constant());
    EXPECT_FLOAT_EQ(1.f, ctx.results[0].constant());
}

TEST_F(event_to_audio_processor_test, rampup_inside_buffer)
{
    auto sut = make_smoother_processor();

    constexpr std::size_t offset{10};
    ev_in_buf.insert(offset, 1.f);

    sut->process(ctx);

    ASSERT_TRUE(ctx.results[0].is_buffer());
    EXPECT_EQ(out0.data(), ctx.results[0].buffer().data());

    EXPECT_TRUE(std::ranges::all_of(
            out0.begin(),
            std::next(out0.begin(), offset),
            equal_to(0.f)));

    EXPECT_TRUE(algorithm::all_of_adjacent(
            std::next(out0.begin(), offset),
            std::next(
                    out0.begin(),
                    offset + default_smooth_length),
            std::less{}));

    EXPECT_TRUE(std::ranges::all_of(
            std::next(
                    out0.begin(),
                    offset + default_smooth_length),
            out0.end(),
            equal_to(1.f)));
}

TEST_F(event_to_audio_processor_test, noramp_if_same_target)
{
    auto sut = make_smoother_processor();

    constexpr std::size_t offset{10};
    ev_in_buf.insert(offset, 0.f);

    sut->process(ctx);

    ASSERT_TRUE(ctx.results[0].is_buffer());
    EXPECT_TRUE(std::ranges::all_of(out0, equal_to(0.f)));
}

TEST_F(event_to_audio_processor_test, ramp_over_process_boundary)
{
    auto sut = make_smoother_processor();

    constexpr std::size_t offset{10};
    ev_in_buf.insert(buffer_size - offset, 1.f);

    sut->process(ctx);

    ASSERT_TRUE(ctx.results[0].is_buffer());
    EXPECT_EQ(out0.data(), ctx.results[0].buffer().data());

    EXPECT_TRUE(std::ranges::all_of(
            out0.begin(),
            std::next(out0.begin(), buffer_size - offset),
            equal_to(0.f)));

    EXPECT_TRUE(algorithm::all_of_adjacent(
            std::next(out0.begin(), buffer_size - offset),
            out0.end(),
            std::less{}));

    ev_in_buf.clear();
    sut->process(ctx);

    EXPECT_TRUE(algorithm::all_of_adjacent(
            out0.begin(),
            std::next(
                    out0.begin(),
                    default_smooth_length - offset + 1), // +1?
            std::less{}));

    EXPECT_TRUE(std::ranges::all_of(
            std::next(
                    out0.begin(),
                    default_smooth_length - offset + 1), // +1?
            out0.end(),
            equal_to(1.f)));
}

} // namespace piejam::audio::engine::test
