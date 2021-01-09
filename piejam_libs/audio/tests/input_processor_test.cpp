// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/input_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice.h>

#include <piejam/container/table.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(input_processor, input_table_is_propagated_to_outputs)
{
    container::table<float> data(2, 16);
    input_processor sut(2);
    sut.set_input(as_const(data.rows()));

    std::array<float, 16> out_buf{};
    std::vector<std::span<float>> outputs{out_buf, out_buf};
    std::vector<audio_slice> results(2);
    sut.process({{}, outputs, results, {}, {}, 16});

    ASSERT_TRUE(results[0].is_buffer());
    EXPECT_EQ(results[0].buffer().data(), data.rows()[0].data());
    EXPECT_EQ(results[0].buffer().size(), data.rows()[0].size());
    ASSERT_TRUE(results[1].is_buffer());
    EXPECT_EQ(results[1].buffer().data(), data.rows()[1].data());
    EXPECT_EQ(results[1].buffer().size(), data.rows()[1].size());
}

} // namespace piejam::audio::engine::test
