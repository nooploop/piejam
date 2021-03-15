// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/mix_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>

#include <xsimd/config/xsimd_align.hpp>

#include <gtest/gtest.h>

#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

struct mix_processor_2_inputs : public ::testing::Test
{
    audio_slice in1;
    audio_slice in2;
    std::array<std::reference_wrapper<audio_slice const>, 2> inputs{in1, in2};
    std::array<float, 4> out_buf{0.f, 0.f};
    std::array<std::span<float>, 1> outputs{out_buf};
    std::array<audio_slice, 1> results{outputs[0]};
    event_input_buffers ev_ins;
    event_output_buffers ev_outs{};
    process_context ctx{inputs, outputs, results, ev_ins, ev_outs, 4};

    std::unique_ptr<processor> sut{make_mix_processor(2)};
};

TEST_F(mix_processor_2_inputs, mixing_constants_will_result_in_constant)
{
    in1 = {.23f};
    in2 = {.58f};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_constant());
    EXPECT_FLOAT_EQ(.23f + .58f, results[0].constant());
}

TEST_F(mix_processor_2_inputs,
       mix_buffer_and_constant_result_will_be_written_into_output_buffer)
{
    std::array in1_buf{.23f, .58f, .77f, .99f};
    in1 = {in1_buf};
    in2 = {.77f};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_buffer());
    ASSERT_EQ(4u, results[0].buffer().size());
    EXPECT_FLOAT_EQ(.23f + .77f, results[0].buffer()[0]);
    EXPECT_FLOAT_EQ(.58f + .77f, results[0].buffer()[1]);
    EXPECT_FLOAT_EQ(.77f + .77f, results[0].buffer()[2]);
    EXPECT_FLOAT_EQ(.99f + .77f, results[0].buffer()[3]);
}

TEST_F(mix_processor_2_inputs,
       mix_buffer_and_silence_result_will_point_to_the_input_buffer)
{
    std::array in1_buf{.23f, .58f, .77f, .99f};
    in1 = {in1_buf};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_buffer());
    EXPECT_EQ(in1_buf.data(), results[0].buffer().data());
    EXPECT_EQ(in1_buf.size(), results[0].buffer().size());
}

TEST_F(mix_processor_2_inputs, mix_two_buffers_result_will_be_in_the_output)
{
    std::array in1_buf{.23f, .58f, .77f, .99f};
    in1 = {in1_buf};
    std::array in2_buf{.99f, .77f, .58f, .23f};
    in2 = {in2_buf};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_buffer());
    ASSERT_EQ(4u, results[0].buffer().size());
    EXPECT_FLOAT_EQ(.23f + .99f, results[0].buffer()[0]);
    EXPECT_FLOAT_EQ(.58f + .77f, results[0].buffer()[1]);
    EXPECT_FLOAT_EQ(.77f + .58f, results[0].buffer()[2]);
    EXPECT_FLOAT_EQ(.99f + .23f, results[0].buffer()[3]);
}

TEST(mix_processor, mix_two_silence_channels)
{
    auto sut = make_mix_processor(2);

    audio_slice const silence;

    std::vector<std::reference_wrapper<audio_slice const>> in(2, silence);
    alignas(XSIMD_DEFAULT_ALIGNMENT) std::array out_buf{0.f};
    std::vector<std::span<float>> out = {out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FALSE(is_silence(result[0]));

    sut->process({in, out, result, {}, {}, 1});

    EXPECT_TRUE(is_silence(result[0]));
}

TEST(mix_processor, mix_one_silence_one_non_silence_channel)
{
    auto sut = make_mix_processor(2);

    audio_slice const silence;
    alignas(XSIMD_DEFAULT_ALIGNMENT) std::array in_buf{0.23f};
    audio_slice in_buf_span(in_buf);

    std::vector<std::reference_wrapper<audio_slice const>> in{
            silence,
            in_buf_span};
    alignas(XSIMD_DEFAULT_ALIGNMENT) std::array out_buf{0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {}, {}, 1});

    ASSERT_EQ(1u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.23f, result[0].buffer()[0]);
}

TEST(mix_processor, mix_two_non_silence_channels)
{
    auto sut = make_mix_processor(2);

    std::array in_buf1{0.23f, 0.23f, 0.23f, 0.23f};
    audio_slice in_buf1_span(in_buf1);
    std::array in_buf2{0.58f, 0.58f, 0.58f, 0.58f};
    audio_slice in_buf2_span(in_buf2);
    std::vector<std::reference_wrapper<audio_slice const>> in{
            in_buf1_span,
            in_buf2_span};
    std::array out_buf{0.f, 0.f, 0.f, 0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {}, {}, 4});

    ASSERT_TRUE(result[0].is_buffer());
    ASSERT_EQ(4u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[0]);
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[1]);
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[2]);
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[3]);
}

TEST(mix_processor, mix_two_silence_one_non_silence_channel)
{
    auto sut = make_mix_processor(3);

    audio_slice const silence;
    alignas(XSIMD_DEFAULT_ALIGNMENT) std::array in_buf{0.23f};
    audio_slice in_buf_span(in_buf);
    std::vector<std::reference_wrapper<audio_slice const>> in{
            silence,
            silence,
            in_buf_span};
    alignas(XSIMD_DEFAULT_ALIGNMENT) std::array out_buf{0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {}, {}, 1});

    ASSERT_TRUE(result[0].is_buffer());
    ASSERT_EQ(1u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.23f, result[0].buffer()[0]);
}

TEST(mix_processor, mix_one_silence_two_non_silence_channels)
{
    auto sut = make_mix_processor(3);

    audio_slice const silence;
    std::array in_buf1{0.23f, 0.23f, 0.23f, 0.23f};
    audio_slice in_buf1_span(in_buf1);
    std::array in_buf2{0.58f, 0.58f, 0.58f, 0.58f};
    audio_slice in_buf2_span(in_buf2);
    std::vector<std::reference_wrapper<audio_slice const>> in{
            in_buf1_span,
            silence,
            in_buf2_span};
    std::array out_buf{0.f, 0.f, 0.f, 0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {}, {}, 4});

    ASSERT_EQ(4u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[0]);
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[1]);
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[2]);
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[3]);
}

struct is_mix_processor_test : ::testing::TestWithParam<std::size_t>
{
};

TEST_P(is_mix_processor_test, verify)
{
    EXPECT_TRUE(is_mix_processor(*make_mix_processor(GetParam())));
}

INSTANTIATE_TEST_SUITE_P(
        verify,
        is_mix_processor_test,
        testing::Range<std::size_t>(2u, 10u));

} // namespace piejam::audio::engine::test
