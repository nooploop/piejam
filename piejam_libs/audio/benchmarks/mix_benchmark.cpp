// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/slice_algorithms.h>

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <numeric>
#include <span>

constexpr auto min_period_size = 16;
constexpr auto max_period_size = 1024;

static auto
mix(std::span<float const> const in1,
    std::span<float const> const in2,
    std::span<float> const dst)
{
    std::transform(
            in1.begin(),
            in1.end(),
            in2.begin(),
            dst.begin(),
            std::plus<float>{});
}

static void
BM_mix_audio_slice_std(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    mipp::vector<float> in_buf1(state.range(0));
    std::iota(in_buf1.begin(), in_buf1.end(), 0.f);
    mipp::vector<float> in_buf2(state.range(0));
    std::iota(in_buf2.begin(), in_buf2.end(), -5.f);
    std::span<float const> in1(in_buf1);
    std::span<float const> in2(in_buf2);

    mipp::vector<float> out_buf(state.range(0));
    std::span<float> out{out_buf};

    for (auto _ : state)
    {
        mix(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_mix_audio_slice_std)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static void
BM_mix_audio_slice(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    mipp::vector<float> in_buf1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf2(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> out_buf(state.range(0));
    std::span<float> out{out_buf};

    auto in1 = piejam::audio::slice<float>(in_buf1);
    auto in2 = piejam::audio::slice<float>(in_buf2);

    for (auto _ : state)
    {
        piejam::audio::add(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_mix_audio_slice)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static auto
multiply_audio_slice_std(
        std::span<float const> const in1,
        std::span<float const> const in2,
        std::span<float> const dst)
{
    std::transform(
            in1.begin(),
            in1.end(),
            in2.begin(),
            dst.begin(),
            std::multiplies<>{});
}

static void
BM_multiply_audio_slice_std(benchmark::State& state)
{
    mipp::vector<float> in_buf1(state.range(0), 1.f);
    mipp::vector<float> in_buf2(state.range(0));
    std::iota(in_buf2.begin(), in_buf2.end(), -5.f);
    std::span<float const> in1(in_buf1);
    std::span<float const> in2(in_buf2);

    mipp::vector<float> out_buf(state.range(0));
    std::span<float> out{out_buf};

    for (auto _ : state)
    {
        multiply_audio_slice_std(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_multiply_audio_slice_std)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static void
BM_multiply_audio_slice(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    mipp::vector<float> in_buf1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf2(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> out_buf(state.range(0));
    std::span<float> out{out_buf};

    auto in1 = piejam::audio::slice<float>(in_buf1);
    auto in2 = piejam::audio::slice<float>(in_buf2);

    for (auto _ : state)
    {
        piejam::audio::multiply(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_multiply_audio_slice)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static void
BM_multiply_audio_slice_by_constant(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    mipp::vector<float> in_bufs1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));

    mipp::vector<float> out_buf(state.range(0));
    std::span<float> out{out_buf};

    auto in1 = piejam::audio::slice<float>(0.75f);
    auto in2 = piejam::audio::slice<float>(in_bufs1);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(piejam::audio::multiply(in1, in2, out));
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_multiply_audio_slice_by_constant)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);
