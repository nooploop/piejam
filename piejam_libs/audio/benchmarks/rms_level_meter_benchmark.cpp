// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/dsp/mipp_iterator.h>
#include <piejam/audio/dsp/rms_level_meter.h>

#include <benchmark/benchmark.h>

#include <cstdlib>
#include <ctime>

static void
BM_rms_level_meter(benchmark::State& state)
{
    piejam::audio::dsp::rms_level_meter<float> lm{
            piejam::audio::sample_rate{48000}};

    std::srand(std::time(nullptr));

    mipp::vector<float> buf(8192);
    std::generate_n(buf.begin(), buf.size(), []() {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    });
    benchmark::ClobberMemory();

    for (auto _ : state)
    {
        std::ranges::copy(
                piejam::audio::dsp::mipp_range(std::span{buf}),
                std::back_inserter(lm));
    }

    benchmark::DoNotOptimize(lm.level());
}

BENCHMARK(BM_rms_level_meter);
