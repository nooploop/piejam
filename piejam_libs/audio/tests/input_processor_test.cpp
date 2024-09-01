// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/input_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/audio/simd.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(input_processor, input_is_propagated_to_outputs)
{
    std::vector<float> data({2.f, 3.f, 5.f, 7.f});
    input_processor sut;

    alignas(mipp::RequiredAlignment) std::array<float, 4> out_buf{};
    std::vector<std::span<float>> outputs{out_buf};
    std::vector<audio_slice> results(1);
    auto converter =
            pcm_input_buffer_converter([&out_buf](std::span<float> const buf) {
                std::ranges::copy(out_buf, buf.begin());
            });
    sut.set_input(converter);
    sut.process({{}, outputs, results, {}, {}, 4});

    ASSERT_TRUE(results[0].is_span());
    EXPECT_EQ(results[0].span().data(), out_buf.data());
    EXPECT_EQ(results[0].span().size(), out_buf.size());
}

} // namespace piejam::audio::engine::test
