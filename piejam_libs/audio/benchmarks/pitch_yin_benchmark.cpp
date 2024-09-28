// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/dsp/pitch_yin.h>

#include <piejam/audio/dsp/generate_sine.h>

#include <mipp.h>

#include <benchmark/benchmark.h>

#include <array>
#include <cstdlib>
#include <ctime>
#include <span>

constexpr std::array freqs{22.34f, 76.53f, 222.33f, 345.67f, 450.99f, 1230.45f};

static void
BM_pitch_yin(benchmark::State& state)
{
    constexpr piejam::audio::sample_rate const sr{48000};

    mipp::vector<float> in_buf(8192);
    piejam::audio::dsp::generate_sine(
            std::span{in_buf},
            sr.as_float(),
            freqs[state.range(0)]);
    benchmark::ClobberMemory();

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
                piejam::audio::dsp::pitch_yin<float>(in_buf, sr));
    }
}

BENCHMARK(BM_pitch_yin)->DenseRange(0, freqs.size() - 1);
