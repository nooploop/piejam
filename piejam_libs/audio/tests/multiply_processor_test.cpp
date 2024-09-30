// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/multiply_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>

#include <mipp.h>

#include <gtest/gtest.h>

#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

struct multiply_processor_2_inputs : public ::testing::Test
{
    constexpr static auto const buffer_size = 8;
    audio_slice in1;
    audio_slice in2;
    std::array<std::reference_wrapper<audio_slice const>, 2> inputs{in1, in2};
    alignas(mipp::RequiredAlignment) std::array<float, buffer_size> out_buf{};
    std::array<std::span<float>, 1> outputs{out_buf};
    std::array<audio_slice, 1> results{outputs[0]};
    event_input_buffers ev_ins;
    event_output_buffers ev_outs{};
    process_context ctx{inputs, outputs, results, ev_ins, ev_outs, buffer_size};

    std::unique_ptr<processor> sut{make_multiply_processor(2)};
};

TEST_F(multiply_processor_2_inputs,
       multiplying_constants_will_result_in_constant)
{
    in1 = {.23f};
    in2 = {.58f};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_constant());
    EXPECT_FLOAT_EQ(.23f * .58f, results[0].constant());
}

TEST_F(multiply_processor_2_inputs,
       multiply_buffer_and_constant_result_will_be_written_into_output_buffer)
{
    alignas(mipp::RequiredAlignment)
            std::array in1_buf{.23f, .58f, .77f, .99f, .23f, .58f, .77f, .99f};
    in1 = {in1_buf};
    in2 = {.77f};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_span());
    ASSERT_EQ(buffer_size, results[0].span().size());
    EXPECT_FLOAT_EQ(.23f * .77f, results[0].span()[0]);
    EXPECT_FLOAT_EQ(.58f * .77f, results[0].span()[1]);
    EXPECT_FLOAT_EQ(.77f * .77f, results[0].span()[2]);
    EXPECT_FLOAT_EQ(.99f * .77f, results[0].span()[3]);
    EXPECT_FLOAT_EQ(.23f * .77f, results[0].span()[4]);
    EXPECT_FLOAT_EQ(.58f * .77f, results[0].span()[5]);
    EXPECT_FLOAT_EQ(.77f * .77f, results[0].span()[6]);
    EXPECT_FLOAT_EQ(.99f * .77f, results[0].span()[7]);
}

TEST_F(multiply_processor_2_inputs,
       multiply_buffer_and_silence_will_result_in_silence)
{
    alignas(mipp::RequiredAlignment)
            std::array in1_buf{.23f, .58f, .77f, .99f, .23f, .58f, .77f, .99f};
    in1 = {in1_buf};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_constant());
    EXPECT_FLOAT_EQ(0.f, results[0].constant());
}

TEST_F(multiply_processor_2_inputs,
       multiply_two_buffers_result_will_be_in_the_output)
{
    alignas(mipp::RequiredAlignment)
            std::array in1_buf{.23f, .58f, .77f, .99f, .23f, .58f, .77f, .99f};
    in1 = {in1_buf};
    alignas(mipp::RequiredAlignment)
            std::array in2_buf{.99f, .77f, .58f, .23f, .99f, .77f, .58f, .23f};
    in2 = {in2_buf};

    sut->process(ctx);

    ASSERT_TRUE(results[0].is_span());
    ASSERT_EQ(buffer_size, results[0].span().size());
    EXPECT_FLOAT_EQ(.23f * .99f, results[0].span()[0]);
    EXPECT_FLOAT_EQ(.58f * .77f, results[0].span()[1]);
    EXPECT_FLOAT_EQ(.77f * .58f, results[0].span()[2]);
    EXPECT_FLOAT_EQ(.99f * .23f, results[0].span()[3]);
    EXPECT_FLOAT_EQ(.23f * .99f, results[0].span()[4]);
    EXPECT_FLOAT_EQ(.58f * .77f, results[0].span()[5]);
    EXPECT_FLOAT_EQ(.77f * .58f, results[0].span()[6]);
    EXPECT_FLOAT_EQ(.99f * .23f, results[0].span()[7]);
}

TEST(mix_processor, multiply_three_non_silence_channels)
{
    auto sut = make_multiply_processor(3);

    constexpr auto const buffer_size = 8u;

    alignas(mipp::RequiredAlignment) std::array<float, buffer_size> in_buf1;
    in_buf1.fill(0.23f);
    audio_slice in_buf1_slice(in_buf1);
    alignas(mipp::RequiredAlignment) std::array<float, buffer_size> in_buf2;
    in_buf2.fill(0.58f);
    audio_slice in_buf2_slice(in_buf2);
    std::vector<std::reference_wrapper<audio_slice const>> in{
            in_buf1_slice,
            in_buf2_slice,
            in_buf2_slice};
    alignas(mipp::RequiredAlignment) std::array<float, buffer_size> out_buf{};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].span()[0]);

    sut->process({in, out, result, {}, {}, buffer_size});

    ASSERT_EQ(buffer_size, result[0].span().size());
    for (auto const v : result[0].span())
    {
        EXPECT_FLOAT_EQ(0.23f * 0.58f * 0.58f, v);
    }
}

TEST(mix_processor, multiply_eight_non_silence_and_one_constant_channel)
{
    auto sut = make_multiply_processor(9);

    constexpr auto const buffer_size = 8u;

    audio_slice const constant_slice(0.58f);
    alignas(mipp::RequiredAlignment) std::array<float, buffer_size> in_buf;
    in_buf.fill(0.23f);
    audio_slice in_buf_slice(in_buf);

    std::vector<std::reference_wrapper<audio_slice const>> in{
            in_buf_slice,
            in_buf_slice,
            in_buf_slice,
            in_buf_slice,
            constant_slice,
            in_buf_slice,
            in_buf_slice,
            in_buf_slice,
            in_buf_slice};
    alignas(mipp::RequiredAlignment) std::array<float, buffer_size> out_buf{};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].span()[0]);

    sut->process({in, out, result, {}, {}, buffer_size});

    ASSERT_EQ(buffer_size, result[0].span().size());
    float const expected = 0.23f * 0.23f * 0.23f * 0.23f * 0.58f * 0.23f *
                           0.23f * 0.23f * 0.23f;
    for (auto const v : result[0].span())
    {
        EXPECT_FLOAT_EQ(expected, v);
    }
}

struct multiply_processor_properties_test
    : ::testing::TestWithParam<std::size_t>
{
};

TEST_P(multiply_processor_properties_test, num_inputs)
{
    EXPECT_EQ(GetParam(), make_multiply_processor(GetParam())->num_inputs());
}

TEST_P(multiply_processor_properties_test, num_outputs)
{
    EXPECT_EQ(1u, make_multiply_processor(GetParam())->num_outputs());
}

TEST_P(multiply_processor_properties_test, event_inputs)
{
    EXPECT_TRUE(make_multiply_processor(GetParam())->event_inputs().empty());
}

TEST_P(multiply_processor_properties_test, event_outputs)
{
    EXPECT_TRUE(make_multiply_processor(GetParam())->event_outputs().empty());
}

INSTANTIATE_TEST_SUITE_P(
        verify,
        multiply_processor_properties_test,
        testing::Range<std::size_t>(2u, 10u));

} // namespace piejam::audio::engine::test
