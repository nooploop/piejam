// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/dsp/rms.h>

#include <benchmark/benchmark.h>

#include <cstdlib>
#include <ctime>
#include <span>

constexpr auto min_period_size = 512;
constexpr auto max_period_size = 8192;

static void
BM_rms(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    mipp::vector<float> in_buf1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    benchmark::ClobberMemory();

    float rms{};
    for (auto _ : state)
    {
        rms = piejam::audio::dsp::rms(std::span{std::as_const(in_buf1)});
        (void)rms;
    }
}

BENCHMARK(BM_rms)->RangeMultiplier(2)->Range(min_period_size, max_period_size);

static void
BM_simd_rms(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    mipp::vector<float> in_buf1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    benchmark::ClobberMemory();

    float rms{};
    for (auto _ : state)
    {
        rms = piejam::audio::dsp::simd::rms(std::span{std::as_const(in_buf1)});
        (void)rms;
    }
}

BENCHMARK(BM_simd_rms)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);
