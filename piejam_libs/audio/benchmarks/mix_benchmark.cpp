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

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <span>
#include <vector>

namespace piejam::audio::engine
{

static auto
mix(std::span<std::span<float const> const> const& src,
    std::span<float> const& dst)
{
    for (std::span<float const> const& in : src)
    {
        if (in.empty())
            continue;

        std::transform(
                in.begin(),
                in.end(),
                dst.begin(),
                dst.begin(),
                std::plus<float>{});
    }
}

static void
BM_mix(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    std::array<std::vector<float>, 2> in_bufs;
    in_bufs[0].resize(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    in_bufs[1].resize(
            state.range(0),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX));

    std::vector<std::span<float const>> ins{in_bufs[0], in_bufs[1]};
    std::span<std::span<float const> const> in{ins};

    std::vector<float> out_buf(state.range(0));
    std::span<float> out{out_buf};

    for (auto _ : state)
        mix(in, out);
}

BENCHMARK(BM_mix)->RangeMultiplier(2)->Range(1, 1024);

} // namespace piejam::audio::engine
