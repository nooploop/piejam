// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/clip_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>

#include <mipp.h>

#include <gtest/gtest.h>

#include <array>

namespace piejam::audio::engine::test
{

struct clip_processor_test : testing::Test
{
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    static constexpr std::size_t buffer_size{4};
    alignas(mipp::RequiredAlignment) std::array<float, buffer_size> out0;
    std::array<std::span<float>, 1> outputs{out0};
    std::array<audio_slice, 1> results;
    audio::engine::process_context ctx{
            .outputs = outputs,
            .results = results,
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = buffer_size};
};

TEST_F(clip_processor_test, properties)
{
    auto sut = make_clip_processor();
    EXPECT_EQ(1u, sut->num_inputs());
    EXPECT_EQ(1u, sut->num_outputs());
    EXPECT_TRUE(sut->event_inputs().empty());
    EXPECT_TRUE(sut->event_outputs().empty());
}

TEST_F(clip_processor_test, input_gets_clipped)
{
    alignas(mipp::RequiredAlignment) std::array in_buf{-1.5f, 1.3f, .5f, -0.7f};
    audio_slice in(in_buf);
    std::array ins{std::cref(in)};
    ctx.inputs = ins;

    auto sut = make_clip_processor();

    sut->process(ctx);

    EXPECT_FLOAT_EQ(-1.f, out0[0]);
    EXPECT_FLOAT_EQ(1.f, out0[1]);
    EXPECT_FLOAT_EQ(.5f, out0[2]);
    EXPECT_FLOAT_EQ(-0.7f, out0[3]);
}

} // namespace piejam::audio::engine::test
