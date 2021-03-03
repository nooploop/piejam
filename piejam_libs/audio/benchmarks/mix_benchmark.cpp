// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <benchmark/benchmark.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/slice_algorithms.h>

#include <xsimd/memory/xsimd_aligned_allocator.hpp>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <numeric>
#include <span>
#include <vector>

using vec_t = std::
        vector<float, xsimd::aligned_allocator<float, XSIMD_DEFAULT_ALIGNMENT>>;
constexpr auto min_period_size = 16;
constexpr auto max_period_size = 1024;

static auto
mix(std::span<float const> const& in1,
    std::span<float const> const& in2,
    std::span<float> const& dst)
{
    std::transform(
            in1.begin(),
            in1.end(),
            in2.begin(),
            dst.begin(),
            std::plus<float>{});
}

static void
BM_mix(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    vec_t in_buf1(state.range(0));
    std::iota(in_buf1.begin(), in_buf1.end(), 0.f);
    vec_t in_buf2(state.range(0));
    std::iota(in_buf2.begin(), in_buf2.end(), -5.f);
    std::span<float const> in1(in_buf1);
    std::span<float const> in2(in_buf2);

    vec_t out_buf(state.range(0));
    std::span<float> out{out_buf};

    for (auto _ : state)
    {
        mix(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_mix)->RangeMultiplier(2)->Range(min_period_size, max_period_size);

static void
BM_mix_audio_slice(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    vec_t in_buf1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    vec_t in_buf2(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    vec_t out_buf(state.range(0));
    std::span<float> out{out_buf};

    auto in1 = piejam::audio::engine::audio_slice(in_buf1);
    auto in2 = piejam::audio::engine::audio_slice(in_buf2);

    for (auto _ : state)
    {
        piejam::audio::engine::add(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_mix_audio_slice)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static auto
multiply_std(
        std::span<float const> const& in1,
        std::span<float const> const& in2,
        std::span<float> const& dst)
{
    std::transform(
            in1.begin(),
            in1.end(),
            in2.begin(),
            dst.begin(),
            std::multiplies<>{});
}

static void
BM_multiply_std(benchmark::State& state)
{
    vec_t in_buf1(state.range(0));
    std::iota(in_buf1.begin(), in_buf1.end(), 0.f);
    vec_t in_buf2(state.range(0));
    std::iota(in_buf2.begin(), in_buf2.end(), -5.f);
    std::span<float const> in1(in_buf1);
    std::span<float const> in2(in_buf2);

    vec_t out_buf(state.range(0));
    std::span<float> out{out_buf};

    for (auto _ : state)
    {
        multiply_std(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_multiply_std)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static auto
multiply(
        std::span<float const> const& in1,
        std::span<float const> const& in2,
        std::span<float> const& dst)
{
    xsimd::transform(
            in1.begin(),
            in1.end(),
            in2.begin(),
            dst.begin(),
            std::multiplies<>{});
}

static void
BM_multiply(benchmark::State& state)
{
    vec_t in_buf1(state.range(0));
    std::iota(in_buf1.begin(), in_buf1.end(), 0.f);
    vec_t in_buf2(state.range(0));
    std::iota(in_buf2.begin(), in_buf2.end(), -5.f);
    std::span<float const> in1(in_buf1);
    std::span<float const> in2(in_buf2);

    vec_t out_buf(state.range(0));
    std::span<float> out{out_buf};

    for (auto _ : state)
    {
        multiply(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_multiply)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static void
BM_multiply_audio_slice(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    vec_t in_buf1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    vec_t in_buf2(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    vec_t out_buf(state.range(0));
    std::span<float> out{out_buf};

    auto in1 = piejam::audio::engine::audio_slice(in_buf1);
    auto in2 = piejam::audio::engine::audio_slice(in_buf2);

    for (auto _ : state)
    {
        piejam::audio::engine::multiply(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_multiply_audio_slice)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);

static void
BM_multiply_by_constant(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    vec_t in_bufs1(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));

    vec_t out_buf(state.range(0));
    std::span<float> out{out_buf};

    auto in1 = piejam::audio::engine::audio_slice(1.f);
    auto in2 = piejam::audio::engine::audio_slice(in_bufs1);

    for (auto _ : state)
    {
        piejam::audio::engine::multiply(in1, in2, out);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_multiply_by_constant)
        ->RangeMultiplier(2)
        ->Range(min_period_size, max_period_size);
