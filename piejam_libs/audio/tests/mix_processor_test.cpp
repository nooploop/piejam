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

#include <piejam/audio/engine/mix_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>

#include <gtest/gtest.h>

#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

TEST(mix_processor, mix_two_silence_channels)
{
    auto sut = make_mix_processor(2);

    audio_slice const silence;

    std::vector<std::reference_wrapper<audio_slice const>> in(2, silence);
    std::array out_buf{0.f};
    std::vector<std::span<float>> out = {out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FALSE(is_silence(result[0]));

    sut->process({in, out, result, {0}, {}, 1});

    EXPECT_TRUE(is_silence(result[0]));
}

TEST(mix_processor, mix_one_silence_one_non_silence_channel)
{
    auto sut = make_mix_processor(2);

    audio_slice const silence;
    std::array in_buf{0.23f};
    audio_slice in_buf_span(in_buf);

    std::vector<std::reference_wrapper<audio_slice const>> in{
            silence,
            in_buf_span};
    std::array out_buf{0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {0}, {}, 1});

    ASSERT_EQ(1u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.23f, result[0].buffer()[0]);
}

TEST(mix_processor, mix_two_non_silence_channels)
{
    auto sut = make_mix_processor(2);

    std::array in_buf1{0.23f};
    audio_slice in_buf1_span(in_buf1);
    std::array in_buf2{0.58f};
    audio_slice in_buf2_span(in_buf2);
    std::vector<std::reference_wrapper<audio_slice const>> in{
            in_buf1_span,
            in_buf2_span};
    std::array out_buf{0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {0}, {}, 1});

    ASSERT_EQ(1u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[0]);
}

TEST(mix_processor, mix_two_silence_one_non_silence_channel)
{
    auto sut = make_mix_processor(3);

    audio_slice const silence;
    std::array in_buf{0.23f};
    audio_slice in_buf_span(in_buf);
    std::vector<std::reference_wrapper<audio_slice const>> in{
            silence,
            silence,
            in_buf_span};
    std::array out_buf{0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {0}, {}, 1});

    ASSERT_TRUE(result[0].is_buffer());
    ASSERT_EQ(1u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.23f, result[0].buffer()[0]);
}

TEST(mix_processor, mix_one_silence_two_non_silence_channels)
{
    auto sut = make_mix_processor(3);

    audio_slice const silence;
    std::array in_buf1{0.23f};
    audio_slice in_buf1_span(in_buf1);
    std::array in_buf2{0.58f};
    audio_slice in_buf2_span(in_buf2);
    std::vector<std::reference_wrapper<audio_slice const>> in{
            in_buf1_span,
            silence,
            in_buf2_span};
    std::array out_buf{0.f};
    std::vector<std::span<float>> out{out_buf};
    std::vector<audio_slice> result{out[0]};

    ASSERT_FLOAT_EQ(0.f, result[0].buffer()[0]);

    sut->process({in, out, result, {0}, {}, 1});

    ASSERT_EQ(1u, result[0].buffer().size());
    EXPECT_FLOAT_EQ(0.81f, result[0].buffer()[0]);
}

} // namespace piejam::audio::engine::test
