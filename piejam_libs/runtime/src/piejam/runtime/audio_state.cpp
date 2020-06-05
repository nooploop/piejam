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

#include <piejam/runtime/audio_state.h>

#include <piejam/algorithm/set_intersection.h>

namespace piejam::runtime
{

template <class Vector>
static auto
set_intersection(Vector const& in, Vector const& out)
{
    Vector result;
    algorithm::set_intersection(in, out, std::back_inserter(result));
    return result;
}

auto
samplerates(
        container::box<audio::pcm_hw_params> input_hw_params,
        container::box<audio::pcm_hw_params> output_hw_params)
        -> audio::samplerates_t
{
    return set_intersection(
            input_hw_params->samplerates,
            output_hw_params->samplerates);
}

auto
samplerates_from_state(audio_state const& state) -> audio::samplerates_t
{
    return samplerates(state.input.hw_params, state.output.hw_params);
}

auto
period_sizes(
        container::box<audio::pcm_hw_params> input_hw_params,
        container::box<audio::pcm_hw_params> output_hw_params)
        -> audio::period_sizes_t
{
    return set_intersection(
            input_hw_params->period_sizes,
            output_hw_params->period_sizes);
}

auto
period_sizes_from_state(audio_state const& state) -> audio::period_sizes_t
{
    return period_sizes(state.input.hw_params, state.output.hw_params);
}

} // namespace piejam::runtime
