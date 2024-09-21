// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/output_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/audio/simd.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(output_processor, input_is_propagated_to_outputs)
{
    mipp::vector<float> data({0.f, 0.f, 0.f, 0.f});
    output_processor sut;
    auto converter = pcm_output_buffer_converter(
            [&data](pcm_output_source_buffer_t const& buf) {
                std::ranges::copy(
                        std::get<std::span<float const>>(buf),
                        data.begin());
            });
    sut.set_output(converter);

    alignas(mipp::RequiredAlignment) std::array<float, 4> in_buf{
            0.23f,
            0.58f,
            0.77f,
            0.91f};
    std::vector<audio_slice> in_spans{in_buf};
    std::vector<std::reference_wrapper<audio_slice const>> inputs{
            in_spans.begin(),
            in_spans.end()};

    sut.process({inputs, {}, {}, {}, {}, 4});

    EXPECT_FLOAT_EQ(0.23f, data[0]);
    EXPECT_FLOAT_EQ(0.58f, data[1]);
    EXPECT_FLOAT_EQ(0.77f, data[2]);
    EXPECT_FLOAT_EQ(0.91f, data[3]);
}

} // namespace piejam::audio::engine::test
