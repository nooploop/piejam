// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/types.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/runtime/audio_engine.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/state.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <future>
#include <vector>

namespace piejam::runtime::test
{

// plot with gnuplot:
//     plot 'add_input_channel.txt' using 1 with lines

struct audio_engine_render_test : public ::testing::Test
{
    audio::sample_rate const sample_rate{48000};
    std::size_t const buffer_size{1024};
    std::vector<float> audio_in_left{std::vector<float>(buffer_size)};
    std::vector<float> audio_in_right{std::vector<float>(buffer_size)};
    std::vector<float> audio_out_left{std::vector<float>(buffer_size)};
    std::vector<float> audio_out_right{std::vector<float>(buffer_size)};
    audio_engine sut{{}, sample_rate, 2, 2};

    std::size_t sine_wave_pos{};
    std::vector<audio::pair<float>> output;

    std::vector<audio::pcm_input_buffer_converter> in_converter{
            audio::pcm_input_buffer_converter{
                    [this](std::span<float> const buffer) {
                        std::ranges::copy(audio_in_left, buffer.begin());
                    }},
            audio::pcm_input_buffer_converter{
                    [this](std::span<float> const buffer) {
                        std::ranges::copy(audio_in_right, buffer.begin());
                    }}};
    std::vector<audio::pcm_output_buffer_converter> out_converter{
            audio::pcm_output_buffer_converter{
                    [this](float c, std::size_t size) {
                        std::ranges::fill_n(audio_out_left.begin(), c, size);
                    },
                    [this](std::span<float const> buffer) {
                        std::ranges::copy(buffer, audio_out_left.begin());
                    }},
            audio::pcm_output_buffer_converter{
                    [this](float c, std::size_t size) {
                        std::ranges::fill_n(audio_out_right.begin(), c, size);
                    },
                    [this](std::span<float const> buffer) {
                        std::ranges::copy(buffer, audio_out_right.begin());
                    }}};

    void fill_sine()
    {
        std::ranges::generate(audio_in_left, [this]() -> float {
            return std::sin(
                    (sine_wave_pos++ / sample_rate.as_float()) * M_PI_2);
        });
    }

    void render_buffer()
    {
        fill_sine();

        sut.process(buffer_size);

        for (std::size_t o = 0; o < buffer_size; ++o)
        {
            output.emplace_back(audio_out_left[o], audio_out_right[o]);
        }
    }

    void render(std::size_t iterations)
    {
        sut.init_process(in_converter, out_converter);
        while (iterations--)
        {
            render_buffer();
        }
    }

    void rebuild(state const& st)
    {
        auto engine_swap = std::async(std::launch::async, [&]() {
            return sut.rebuild(st, {}, nullptr);
        });

        do
        {
            render_buffer();
        } while (engine_swap.wait_for(std::chrono::milliseconds{1}) !=
                 std::future_status::ready);
    }

    void dump_output(std::filesystem::path const& file)
    {
        std::ofstream os(file);
        for (auto const& p : output)
        {
            os << p.left << '\t' << p.right << std::endl;
        }
    }
};

TEST_F(audio_engine_render_test, add_input_channel)
{
    auto st = make_initial_state();

    add_mixer_channel(st, "in1", audio::bus_type::stereo);
    add_mixer_channel(st, "out", audio::bus_type::stereo);

    rebuild(st);
    render(200);

    add_mixer_channel(st, "in2", audio::bus_type::mono);

    rebuild(st);
    render(200);

    dump_output("add_input_channel.txt");
}

} // namespace piejam::runtime::test
