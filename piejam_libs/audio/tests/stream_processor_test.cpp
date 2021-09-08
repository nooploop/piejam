// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/stream_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <vector>

namespace piejam::audio::engine::test
{

struct stream_processor_1_test : testing::Test
{
    event_input_buffers event_ins;
    event_output_buffers event_outs;
    process_context ctx{
            .event_inputs = event_ins,
            .event_outputs = event_outs,
            .buffer_size = 8};
    std::unique_ptr<stream_processor> sut{make_stream_processor(1, 8)};
};

TEST_F(stream_processor_1_test, properties)
{
    EXPECT_EQ(1u, sut->num_inputs());
    EXPECT_EQ(0u, sut->num_outputs());
    EXPECT_TRUE(sut->event_inputs().empty());
    EXPECT_TRUE(sut->event_outputs().empty());
}

TEST_F(stream_processor_1_test, process_buffer_and_consume)
{
    alignas(mipp::RequiredAlignment)
            std::array in_buf{1.f, 1.f, 1.f, 1.f, 2.f, 2.f, 2.f, 2.f};
    audio_slice in(in_buf);
    std::array ins{std::cref(in)};
    ctx.inputs = ins;

    sut->process(ctx);

    std::vector<float> out;
    sut->consume([&out](std::span<float const> const& data) {
        std::ranges::copy(data, std::back_inserter(out));
    });

    EXPECT_TRUE(std::ranges::equal(in_buf, out));
}

TEST_F(stream_processor_1_test, process_constant_and_consume)
{
    audio_slice in(3.f);
    std::array ins{std::cref(in)};
    ctx.inputs = ins;

    sut->process(ctx);

    std::vector<float> out;
    sut->consume([&out](std::span<float const> const& data) {
        std::ranges::copy(data, std::back_inserter(out));
    });

    EXPECT_EQ(8u, out.size());
    EXPECT_TRUE(std::ranges::all_of(out, [](float x) { return x == 3.f; }));
}

TEST_F(stream_processor_1_test, process_twice_buffer_and_consume)
{
    alignas(mipp::RequiredAlignment) std::array in_buf{1.f, 1.f, 1.f, 1.f};
    audio_slice in(in_buf);
    std::array ins{std::cref(in)};
    ctx.buffer_size = 4;
    ctx.inputs = ins;

    sut->process(ctx);

    in_buf = {2.f, 2.f, 2.f, 2.f};
    sut->process(ctx);

    std::vector<float> out;
    sut->consume([&out](std::span<float const> const& data) {
        std::ranges::copy(data, std::back_inserter(out));
    });

    EXPECT_TRUE(std::ranges::equal(
            std::array{1.f, 1.f, 1.f, 1.f, 2.f, 2.f, 2.f, 2.f},
            out));
}

struct stream_processor_2_test : testing::Test
{
    event_input_buffers event_ins;
    event_output_buffers event_outs;
    process_context ctx{
            .event_inputs = event_ins,
            .event_outputs = event_outs,
            .buffer_size = 8};
    std::unique_ptr<stream_processor> sut{make_stream_processor(2, 16)};
};

TEST_F(stream_processor_2_test, properties)
{
    EXPECT_EQ(2u, sut->num_inputs());
    EXPECT_EQ(0u, sut->num_outputs());
    EXPECT_TRUE(sut->event_inputs().empty());
    EXPECT_TRUE(sut->event_outputs().empty());
}

TEST_F(stream_processor_2_test, process_buffer_and_consume)
{
    alignas(mipp::RequiredAlignment)
            std::array in1_buf{1.f, 1.f, 1.f, 1.f, 3.f, 3.f, 3.f, 3.f};
    audio_slice in1(in1_buf);
    alignas(mipp::RequiredAlignment)
            std::array in2_buf{2.f, 2.f, 2.f, 2.f, 4.f, 4.f, 4.f, 4.f};
    audio_slice in2(in2_buf);
    std::array ins{std::cref(in1), std::cref(in2)};
    ctx.inputs = ins;

    sut->process(ctx);

    std::vector<float> out;
    sut->consume([&out](std::span<float const> const& data) {
        std::ranges::copy(data, std::back_inserter(out));
    });

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f,
            3.f,
            4.f,
            3.f,
            4.f,
            3.f,
            4.f,
            3.f,
            4.f))(out));
}

TEST_F(stream_processor_2_test, process_constants_and_consume)
{
    audio_slice in1(1.f);
    audio_slice in2(2.f);
    std::array ins{std::cref(in1), std::cref(in2)};
    ctx.inputs = ins;

    sut->process(ctx);

    std::vector<float> out;
    sut->consume([&out](std::span<float const> const& data) {
        std::ranges::copy(data, std::back_inserter(out));
    });

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f,
            1.f,
            2.f))(out));
}

struct stream_processor_4_test : testing::Test
{
    event_input_buffers event_ins;
    event_output_buffers event_outs;
    process_context ctx{
            .event_inputs = event_ins,
            .event_outputs = event_outs,
            .buffer_size = 8};
    std::unique_ptr<stream_processor> sut{make_stream_processor(4, 16)};
};

TEST_F(stream_processor_4_test, properties)
{
    EXPECT_EQ(4u, sut->num_inputs());
    EXPECT_EQ(0u, sut->num_outputs());
    EXPECT_TRUE(sut->event_inputs().empty());
    EXPECT_TRUE(sut->event_outputs().empty());
}

TEST_F(stream_processor_4_test, process_buffer_and_consume)
{
    alignas(mipp::RequiredAlignment)
            std::array in1_buf{1.f, 1.f, 1.f, 1.f, 3.f, 3.f, 3.f, 3.f};
    audio_slice in1(in1_buf);
    alignas(mipp::RequiredAlignment)
            std::array in2_buf{2.f, 2.f, 2.f, 2.f, 4.f, 4.f, 4.f, 4.f};
    audio_slice in2(in2_buf);
    alignas(mipp::RequiredAlignment)
            std::array in3_buf{5.f, 5.f, 5.f, 5.f, 7.f, 7.f, 7.f, 7.f};
    audio_slice in3(in3_buf);
    alignas(mipp::RequiredAlignment)
            std::array in4_buf{6.f, 6.f, 6.f, 6.f, 8.f, 8.f, 8.f, 8.f};
    audio_slice in4(in4_buf);

    std::array ins{
            std::cref(in1),
            std::cref(in2),
            std::cref(in3),
            std::cref(in4)};
    ctx.inputs = ins;

    sut->process(ctx);

    std::vector<float> out;
    sut->consume([&out](std::span<float const> const& data) {
        std::ranges::copy(data, std::back_inserter(out));
    });

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            2.f,
            5.f,
            6.f,
            1.f,
            2.f,
            5.f,
            6.f,
            1.f,
            2.f,
            5.f,
            6.f,
            1.f,
            2.f,
            5.f,
            6.f,
            3.f,
            4.f,
            7.f,
            8.f,
            3.f,
            4.f,
            7.f,
            8.f,
            3.f,
            4.f,
            7.f,
            8.f,
            3.f,
            4.f,
            7.f,
            8.f))(out));
}

struct stream_processor_n_test : testing::Test
{
    event_input_buffers event_ins;
    event_output_buffers event_outs;
    process_context ctx{
            .event_inputs = event_ins,
            .event_outputs = event_outs,
            .buffer_size = 4};
    std::unique_ptr<stream_processor> sut{make_stream_processor(5, 4)};
};

TEST_F(stream_processor_n_test, properties)
{
    EXPECT_EQ(5u, sut->num_inputs());
    EXPECT_EQ(0u, sut->num_outputs());
    EXPECT_TRUE(sut->event_inputs().empty());
    EXPECT_TRUE(sut->event_outputs().empty());
}

TEST_F(stream_processor_n_test, process_buffer_and_consume)
{
    alignas(mipp::RequiredAlignment) std::array in1_buf{1.f, 2.f, 3.f, 4.f};
    audio_slice in1(in1_buf);
    alignas(mipp::RequiredAlignment) std::array in2_buf{5.f, 6.f, 7.f, 8.f};
    audio_slice in2(in2_buf);
    alignas(mipp::RequiredAlignment) std::array in3_buf{-1.f, -2.f, -3.f, -4.f};
    audio_slice in3(in3_buf);
    alignas(mipp::RequiredAlignment) std::array in4_buf{9.f, 9.f, -9.f, -9.f};
    audio_slice in4(in4_buf);
    audio_slice in5(0.f);

    std::array ins{
            std::cref(in1),
            std::cref(in2),
            std::cref(in3),
            std::cref(in4),
            std::cref(in5)};
    ctx.inputs = ins;

    sut->process(ctx);

    std::vector<float> out;
    sut->consume([&out](std::span<float const> const& data) {
        std::ranges::copy(data, std::back_inserter(out));
    });

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            5.f,
            -1.f,
            9.f,
            0.f,
            2.f,
            6.f,
            -2.f,
            9.f,
            0.f,
            3.f,
            7.f,
            -3.f,
            -9.f,
            0.f,
            4.f,
            8.f,
            -4.f,
            -9.f,
            0.f))(out));
}

} // namespace piejam::audio::engine::test
