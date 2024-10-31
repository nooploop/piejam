// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/mix_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/slice.h>

#include <mipp.h>

#include <benchmark/benchmark.h>

#include <cstdlib>
#include <ctime>
#include <vector>

namespace piejam::audio::engine
{

static void
BM_mix_processor_2_inputs(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    auto sut = make_mix_processor(2);

    std::size_t const buffer_size = state.range(0);

    mipp::vector<float> in_buf1(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf2(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    std::vector<slice<float>> in_slices{in_buf1, in_buf2};
    std::vector<std::reference_wrapper<slice<float> const>> in(
            in_slices.begin(),
            in_slices.end());

    mipp::vector<float> out_buf(buffer_size);
    std::vector<std::span<float>> out{out_buf};

    std::vector<slice<float>> res{{}};

    process_context ctx{in, out, res, {}, {}, buffer_size};

    for (auto _ : state)
    {
        sut->process(ctx);
        benchmark::ClobberMemory();
    }
}

static void
BM_mix_processor_3_inputs(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    auto sut = make_mix_processor(3);

    std::size_t const buffer_size = state.range(0);

    mipp::vector<float> in_buf1(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf2(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf3(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    std::vector<slice<float>> in_slices{in_buf1, in_buf2, in_buf3};
    std::vector<std::reference_wrapper<slice<float> const>> in(
            in_slices.begin(),
            in_slices.end());

    mipp::vector<float> out_buf(buffer_size);
    std::vector<std::span<float>> out{out_buf};

    std::vector<slice<float>> res{{}};

    process_context ctx{in, out, res, {}, {}, buffer_size};

    for (auto _ : state)
    {
        sut->process(ctx);
        benchmark::ClobberMemory();
    }
}

static void
BM_mix_processor_4_inputs(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    auto sut = make_mix_processor(4);

    std::size_t const buffer_size = state.range(0);

    mipp::vector<float> in_buf1(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf2(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf3(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    mipp::vector<float> in_buf4(
            buffer_size,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    std::vector<slice<float>> in_slices{in_buf1, in_buf2, in_buf3, in_buf4};
    std::vector<std::reference_wrapper<slice<float> const>> in(
            in_slices.begin(),
            in_slices.end());

    mipp::vector<float> out_buf(buffer_size);
    std::vector<std::span<float>> out{out_buf};

    std::vector<slice<float>> res{{}};

    process_context ctx{in, out, res, {}, {}, buffer_size};

    for (auto _ : state)
    {
        sut->process(ctx);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_mix_processor_2_inputs)
        ->RangeMultiplier(2)
        ->Range(mipp::N<float>(), 1024);
BENCHMARK(BM_mix_processor_3_inputs)
        ->RangeMultiplier(2)
        ->Range(mipp::N<float>(), 1024);
BENCHMARK(BM_mix_processor_4_inputs)
        ->RangeMultiplier(2)
        ->Range(mipp::N<float>(), 1024);

} // namespace piejam::audio::engine
