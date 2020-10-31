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

#include <piejam/audio/components/level_meter_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>

#include <gtest/gtest.h>

#include <array>
#include <span>
#include <vector>

namespace piejam::audio::components::test
{

struct level_meter_processor_test : ::testing::Test
{
    level_meter_processor sut{4800};
    std::array<float, 2> in_buf{};
    std::vector<engine::audio_slice> in_buf_spans{in_buf};
    std::vector<std::reference_wrapper<engine::audio_slice const>> in_bufs{
            in_buf_spans.begin(),
            in_buf_spans.end()};
};

TEST_F(level_meter_processor_test, new_peak_is_meassured_from_input_buffer)
{
    in_buf[0] = .7f;
    in_buf_spans[0] = std::span{in_buf.data(), 1};

    sut.process({in_bufs, {}, {}, {}, {}, 1});

    EXPECT_FLOAT_EQ(.7f, sut.peak_level());
}

TEST_F(level_meter_processor_test,
       second_sample_with_higher_level_will_be_the_new_peak)
{
    in_buf[0] = .7f;
    in_buf[1] = .8f;

    sut.process({in_bufs, {}, {}, {}, {}, 2});

    EXPECT_FLOAT_EQ(.8f, sut.peak_level());
}

TEST_F(level_meter_processor_test,
       second_sample_with_lower_level_than_decayed_level_has_no_impact_on_peak)
{
    in_buf[0] = .7f;
    in_buf[1] = .5f;

    sut.process({in_bufs, {}, {}, {}, {}, 2});

    EXPECT_LT(sut.peak_level(), .7f);
}

} // namespace piejam::audio::components::test
