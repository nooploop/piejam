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

#include <piejam/runtime/mixer.h>

#include <benchmark/benchmark.h>

namespace piejam::runtime
{

static void
BM_copy_mixer_state_benchmark(benchmark::State& state)
{
    mixer::state st;
    mixer::add_bus(
            st.buses,
            st.inputs,
            mixer::bus{.name = "in1", .type = audio::bus_type::mono});
    mixer::add_bus(
            st.buses,
            st.inputs,
            mixer::bus{.name = "in2", .type = audio::bus_type::mono});
    mixer::add_bus(
            st.buses,
            st.inputs,
            mixer::bus{.name = "in3", .type = audio::bus_type::mono});
    mixer::add_bus(
            st.buses,
            st.outputs,
            mixer::bus{.name = "main", .type = audio::bus_type::stereo});

    for (auto _ : state)
    {
        auto new_st = st;
        benchmark::DoNotOptimize(new_st);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_copy_mixer_state_benchmark);

} // namespace piejam::runtime
