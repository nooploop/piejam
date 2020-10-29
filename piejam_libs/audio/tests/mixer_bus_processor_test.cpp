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

#include <piejam/audio/components/mixer_bus_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/pan.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <span>
#include <vector>

namespace piejam::audio::components::test
{

struct mixer_bus_processor_test : ::testing::Test
{
    std::size_t const buffer_size{16};
    engine::event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_mem{&ev_buf_mem.memory_resource()};
    engine::event_buffer<float> ev_in_pan_buf{ev_mem};
    engine::event_buffer<float> ev_in_vol_buf{ev_mem};
    engine::event_input_buffers ev_in_bufs{2};
    engine::event_output_buffers ev_out_bufs;
    std::unique_ptr<engine::processor> sut{make_mono_mixer_bus_processor()};

    mixer_bus_processor_test()
    {
        ev_in_bufs.set(0, ev_in_pan_buf);
        ev_in_bufs.set(1, ev_in_vol_buf);
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
        sut->create_event_output_buffers(ev_out_bufs);
    }
};

TEST_F(mixer_bus_processor_test, empty_inputs_to_empty_outputs)
{
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    EXPECT_TRUE(ev_out_bufs.get<float>(0).empty());
    EXPECT_TRUE(ev_out_bufs.get<float>(1).empty());
}

TEST_F(mixer_bus_processor_test, one_pan_event)
{
    ev_in_pan_buf.insert(1, 1.f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(1.f).left}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(1.f).right}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, two_pan_events_at_different_offsets)
{
    ev_in_pan_buf.insert(1, 1.f);
    ev_in_pan_buf.insert(3, -1.f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(1.f).left},
            {3, sinusoidal_constant_power_pan(-1.f).left}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(1.f).right},
            {3, sinusoidal_constant_power_pan(-1.f).right}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, two_pan_events_at_same_offset)
{
    ev_in_pan_buf.insert(1, 1.f);
    ev_in_pan_buf.insert(1, -1.f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(1.f).left},
            {1, sinusoidal_constant_power_pan(-1.f).left}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(1.f).right},
            {1, sinusoidal_constant_power_pan(-1.f).right}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, one_vol_event)
{
    ev_in_vol_buf.insert(1, .5f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(0.f).left * .5f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(0.f).right * .5f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, two_vol_events_at_different_offsets)
{
    ev_in_vol_buf.insert(1, .5f);
    ev_in_vol_buf.insert(3, .7f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(0.f).left * .5f},
            {3, sinusoidal_constant_power_pan(0.f).left * .7f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(0.f).right * .5f},
            {3, sinusoidal_constant_power_pan(0.f).right * .7f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, two_vol_events_at_same_offset)
{
    ev_in_vol_buf.insert(1, .5f);
    ev_in_vol_buf.insert(1, .7f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(0.f).left * .5f},
            {1, sinusoidal_constant_power_pan(0.f).left * .7f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(0.f).right * .5f},
            {1, sinusoidal_constant_power_pan(0.f).right * .7f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, first_pan_then_vol)
{
    ev_in_pan_buf.insert(1, 1.f);
    ev_in_vol_buf.insert(3, .7f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(1.f).left},
            {3, sinusoidal_constant_power_pan(1.f).left * .7f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(1.f).right},
            {3, sinusoidal_constant_power_pan(1.f).right * .7f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, first_vol_then_pan)
{
    ev_in_vol_buf.insert(1, .7f);
    ev_in_pan_buf.insert(3, 1.f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(0.f).left * .7f},
            {3, sinusoidal_constant_power_pan(1.f).left * .7f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(0.f).right * .7f},
            {3, sinusoidal_constant_power_pan(1.f).right * .7f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, pan_and_vol_at_same_offset)
{
    ev_in_vol_buf.insert(1, .7f);
    ev_in_pan_buf.insert(1, 1.f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(1.f).left * .7f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(1.f).right * .7f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test,
       pan_and_vol_at_same_offset_twice_consecutive)
{
    ev_in_vol_buf.insert(1, .7f);
    ev_in_pan_buf.insert(1, 1.f);
    ev_in_vol_buf.insert(3, .5f);
    ev_in_pan_buf.insert(3, -1.f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(1.f).left * .7f},
            {3, sinusoidal_constant_power_pan(-1.f).left * .5f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(1.f).right * .7f},
            {3, sinusoidal_constant_power_pan(-1.f).right * .5f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test,
       pan_and_vol_at_same_offset_twice_at_same_offset)
{
    ev_in_vol_buf.insert(1, .7f);
    ev_in_pan_buf.insert(1, 1.f);
    ev_in_vol_buf.insert(1, .5f);
    ev_in_pan_buf.insert(1, -1.f);
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, sinusoidal_constant_power_pan(1.f).left * .7f},
            {1, sinusoidal_constant_power_pan(-1.f).left * .5f}};
    std::vector<engine::event<float>> expected_right{
            {1, sinusoidal_constant_power_pan(1.f).right * .7f},
            {1, sinusoidal_constant_power_pan(-1.f).right * .5f}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

TEST_F(mixer_bus_processor_test, stereo_balance)
{
    ev_in_pan_buf.insert(1, 1.f);
    ev_in_pan_buf.insert(3, -1.f);
    ev_in_pan_buf.insert(5, 0.f);
    sut = make_stereo_mixer_bus_processor();
    sut->process({{}, {}, {}, ev_in_bufs, ev_out_bufs, buffer_size});

    auto const& ev_out_left = ev_out_bufs.get<float>(0);
    auto const& ev_out_right = ev_out_bufs.get<float>(1);

    std::vector<engine::event<float>> expected_left{
            {1, stereo_balance(1.f).left},
            {3, stereo_balance(-1.f).left},
            {5, stereo_balance(0.f).left}};
    std::vector<engine::event<float>> expected_right{
            {1, stereo_balance(1.f).right},
            {3, stereo_balance(-1.f).right},
            {5, stereo_balance(0.f).right}};

    EXPECT_TRUE(std::ranges::equal(ev_out_left, expected_left));
    EXPECT_TRUE(std::ranges::equal(ev_out_right, expected_right));
}

} // namespace piejam::audio::components::test
