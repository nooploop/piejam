// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/ladspa/scan.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/actions/finalize_ladspa_fx_plugin_scan.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/state.h>

#include <benchmark/benchmark.h>

namespace piejam::runtime
{

static void
BM_copy_state_benchmark(benchmark::State& bench_state)
{
    state st;
    auto in1 = add_mixer_channel(st, "In1yohohofoobarbaz");
    insert_internal_fx_module(st, in1, npos, fx::internal::tool, {}, {});
    auto in2 = add_mixer_channel(st, "In2");
    insert_internal_fx_module(st, in2, npos, fx::internal::tool, {}, {});
    auto in3 = add_mixer_channel(st, "In3");
    insert_internal_fx_module(st, in3, npos, fx::internal::tool, {}, {});
    auto out = add_mixer_channel(st, "out");
    insert_internal_fx_module(st, out, npos, fx::internal::tool, {}, {});

    actions::finalize_ladspa_fx_plugin_scan ladspa_fx_scan;
    ladspa_fx_scan.plugins = ladspa::scan_directory("/usr/lib/ladspa");
    st = ladspa_fx_scan.reduce(st);

    for (auto _ : bench_state)
    {
        auto new_st = st;
        benchmark::DoNotOptimize(new_st);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_copy_state_benchmark);

static void
BM_get_bus_name_benchmark(benchmark::State& bench_state)
{
    state st;
    auto in1 = add_mixer_channel(st, "In1");
    insert_internal_fx_module(st, in1, npos, fx::internal::tool, {}, {});
    auto in2 = add_mixer_channel(st, "In2");
    insert_internal_fx_module(st, in2, npos, fx::internal::tool, {}, {});
    auto in3 = add_mixer_channel(st, "In3");
    insert_internal_fx_module(st, in3, npos, fx::internal::tool, {}, {});
    auto out = add_mixer_channel(st, "out");
    insert_internal_fx_module(st, out, npos, fx::internal::tool, {}, {});

    auto const get_name = selectors::make_mixer_channel_name_selector(in1);

    for (auto _ : bench_state)
    {
        auto name = get_name.get(st);
        benchmark::DoNotOptimize(name);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_get_bus_name_benchmark);

} // namespace piejam::runtime
