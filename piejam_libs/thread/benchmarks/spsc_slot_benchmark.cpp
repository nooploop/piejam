// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <benchmark/benchmark.h>

#include <piejam/thread/spsc_slot.h>

#include <iostream>
#include <thread>

static void
BM_spsc_slot_int(benchmark::State& state)
{
    piejam::thread::spsc_slot<int> slot;
    std::jthread pusher([i = int{}, &slot](std::stop_token stoken) mutable {
        while (!stoken.stop_requested())
        {
            slot.push(i++);
        }
    });

    int r{};
    for (auto _ : state)
    {
        while (!slot.pull(r))
        {
            ;
        }
    }
}

BENCHMARK(BM_spsc_slot_int)->Repetitions(10);
