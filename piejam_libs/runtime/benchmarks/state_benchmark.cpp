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

#include <piejam/audio/ladspa/scan.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/actions/finalize_ladspa_fx_plugin_scan.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/selectors.h>

#include <benchmark/benchmark.h>

namespace piejam::runtime
{

static void
BM_copy_state_benchmark(benchmark::State& state)
{
    audio_state st;
    auto in1 = add_mixer_bus<audio::bus_direction::input>(
            st,
            "In1yohohofoobarbaz",
            audio::bus_type::mono);
    insert_internal_fx_module(st, in1, npos, fx::internal::gain);
    auto in2 = add_mixer_bus<audio::bus_direction::input>(
            st,
            "In2",
            audio::bus_type::mono);
    insert_internal_fx_module(st, in2, npos, fx::internal::gain);
    auto in3 = add_mixer_bus<audio::bus_direction::input>(
            st,
            "In3",
            audio::bus_type::mono);
    insert_internal_fx_module(st, in3, npos, fx::internal::gain);
    auto out = add_mixer_bus<audio::bus_direction::output>(
            st,
            "out",
            audio::bus_type::stereo);
    insert_internal_fx_module(st, out, npos, fx::internal::gain);

    actions::finalize_ladspa_fx_plugin_scan ladspa_fx_scan;
    ladspa_fx_scan.plugins = audio::ladspa::scan_directory("/usr/lib/ladspa");
    st = ladspa_fx_scan.reduce(st);

    for (auto _ : state)
    {
        auto new_st = st;
        benchmark::DoNotOptimize(new_st);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_copy_state_benchmark);

static void
BM_get_bus_name_benchmark(benchmark::State& state)
{
    audio_state st;
    auto in1 = add_mixer_bus<audio::bus_direction::input>(
            st,
            "In1",
            audio::bus_type::mono);
    insert_internal_fx_module(st, in1, npos, fx::internal::gain);
    auto in2 = add_mixer_bus<audio::bus_direction::input>(
            st,
            "In2",
            audio::bus_type::mono);
    insert_internal_fx_module(st, in2, npos, fx::internal::gain);
    auto in3 = add_mixer_bus<audio::bus_direction::input>(
            st,
            "In3",
            audio::bus_type::mono);
    insert_internal_fx_module(st, in3, npos, fx::internal::gain);
    auto out = add_mixer_bus<audio::bus_direction::output>(
            st,
            "out",
            audio::bus_type::stereo);
    insert_internal_fx_module(st, out, npos, fx::internal::gain);

    auto const get_name = selectors::make_bus_name_selector(in1);

    for (auto _ : state)
    {
        auto name = get_name.get(st);
        benchmark::DoNotOptimize(name);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_get_bus_name_benchmark);

} // namespace piejam::runtime
