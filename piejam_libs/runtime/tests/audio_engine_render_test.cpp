// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/table.h>
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

namespace piejam::runtime::test
{

// plot with gnuplot:
//     plot 'add_input_channel.txt' using 1 with lines

struct audio_engine_render_test : public ::testing::Test
{
    audio::samplerate_t const samplerate{48000};
    std::size_t const buffer_size{1024};
    audio::table audio_in{2, buffer_size};
    audio::table audio_out{2, buffer_size};
    audio_engine sut{{}, samplerate, 2, 2};

    std::size_t sine_wave_pos{};
    std::vector<audio::pair<float>> output;

    std::vector<audio::pcm_input_buffer_converter> in_converter{
            [this](std::span<float> const& buffer) {
                std::ranges::copy(audio_in.rows()[0], buffer.begin());
            },
            [this](std::span<float> const& buffer) {
                std::ranges::copy(audio_in.rows()[1], buffer.begin());
            }};
    std::vector<audio::pcm_output_buffer_converter> out_converter{
            [this](std::span<float const> const& buffer) {
                std::ranges::copy(buffer, audio_out.rows()[0].begin());
            },
            [this](std::span<float const> const& buffer) {
                std::ranges::copy(buffer, audio_out.rows()[1].begin());
            }};

    void fill_sine()
    {
        std::ranges::generate(audio_in.rows()[0], [this]() -> float {
            return std::sin(
                    (sine_wave_pos++ / static_cast<float>(samplerate)) *
                    M_PI_2);
        });
    }

    void render_buffer()
    {
        fill_sine();

        sut(in_converter, out_converter, buffer_size);

        for (std::size_t o = 0; o < buffer_size; ++o)
            output.emplace_back(audio_out.rows()[0][o], audio_out.rows()[1][o]);
    }

    void render(std::size_t iterations)
    {
        while (iterations--)
            render_buffer();
    }

    void rebuild(state const& st)
    {
        auto engine_swap = std::async(std::launch::async, [&]() {
            return sut.rebuild(
                    st.mixer_state,
                    st.device_io_state.buses,
                    st.fx_modules,
                    st.fx_parameters,
                    st.params,
                    {},
                    nullptr,
                    false,
                    {});
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
            os << p.left << '\t' << p.right << std::endl;
    }
};

TEST_F(audio_engine_render_test, add_input_channel)
{
    auto st = make_initial_state();

    add_mixer_channel(st, "in1");
    add_mixer_channel(st, "out");

    rebuild(st);
    render(200);

    add_mixer_channel(st, "in2");

    rebuild(st);
    render(200);

    dump_output("add_input_channel.txt");
}

} // namespace piejam::runtime::test
