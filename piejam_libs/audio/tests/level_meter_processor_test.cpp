// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/level_meter_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice.h>

#include <gtest/gtest.h>

#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

struct level_meter_processor_test : ::testing::Test
{
    level_meter_processor sut{4800};
    std::array<float, 2> in_buf{};
    std::vector<audio_slice> in_buf_spans{in_buf};
    std::vector<std::reference_wrapper<audio_slice const>> in_bufs{
            in_buf_spans.begin(),
            in_buf_spans.end()};
    event_output_buffers ev_out_bufs;

    level_meter_processor_test()
    {
        ev_out_bufs.set_event_memory(std::pmr::get_default_resource());
        for (auto const& port : sut.event_outputs())
            ev_out_bufs.add(port);
    }
};

TEST_F(level_meter_processor_test, new_peak_is_meassured_from_input_buffer)
{
    in_buf[0] = .7f;
    in_buf_spans[0] = std::span{in_buf.data(), 1};

    sut.process({in_bufs, {}, {}, {}, ev_out_bufs, 1});

    ASSERT_EQ(1u, ev_out_bufs.get<float>(0).size());
    EXPECT_FLOAT_EQ(.7f, ev_out_bufs.get<float>(0).begin()->value());
}

TEST_F(level_meter_processor_test,
       second_sample_with_higher_level_will_be_the_new_peak)
{
    in_buf[0] = .7f;
    in_buf[1] = .8f;

    sut.process({in_bufs, {}, {}, {}, ev_out_bufs, 2});

    ASSERT_EQ(1u, ev_out_bufs.get<float>(0).size());
    EXPECT_FLOAT_EQ(.8f, ev_out_bufs.get<float>(0).begin()->value());
}

TEST_F(level_meter_processor_test,
       second_sample_with_lower_level_than_decayed_level_has_no_impact_on_peak)
{
    in_buf[0] = .7f;
    in_buf[1] = .5f;

    sut.process({in_bufs, {}, {}, {}, ev_out_bufs, 2});

    ASSERT_EQ(1u, ev_out_bufs.get<float>(0).size());
    EXPECT_LT(ev_out_bufs.get<float>(0).begin()->value(), .7f);
}

} // namespace piejam::audio::engine::test
