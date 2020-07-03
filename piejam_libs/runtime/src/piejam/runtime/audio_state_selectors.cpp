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

#include <piejam/runtime/audio_state_selectors.h>

#include <piejam/container/boxify_result.h>
#include <piejam/functional/memo.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::audio_state_selectors
{

const selector<samplerate> select_samplerate(
        [get_samplerates =
                 memoized(container::boxify_result(&runtime::samplerates))](
                audio_state const& st) mutable -> samplerate {
            return {get_samplerates(st.input.hw_params, st.output.hw_params),
                    st.samplerate};
        });

const selector<period_size> select_period_size(
        [get_period_sizes =
                 memoized(container::boxify_result(&runtime::period_sizes))](
                audio_state const& st) mutable -> period_size {
            return {get_period_sizes(st.input.hw_params, st.output.hw_params),
                    st.period_size};
        });

const selector<input_devices>
        select_input_devices([](audio_state const& st) -> input_devices {
            return {st.pcm_devices, st.input.index};
        });

const selector<output_devices>
        select_output_devices([](audio_state const& st) -> output_devices {
            return {st.pcm_devices, st.output.index};
        });

const selector<std::size_t>
        select_num_input_devices([](audio_state const& st) -> std::size_t {
            return st.input.hw_params->num_channels;
        });

auto
make_input_gain_selector(std::size_t const index) -> selector<float>
{
    return selector<float>([index](audio_state const& st) -> float {
        return index < st.mixer_state.inputs.size()
                       ? st.mixer_state.inputs[index].gain
                       : 0.f;
    });
}

auto
make_input_pan_selector(std::size_t const index) -> selector<float>
{
    return selector<float>([index](audio_state const& st) -> float {
        return index < st.mixer_state.inputs.size()
                       ? st.mixer_state.inputs[index].pan
                       : 0.f;
    });
}

auto
make_input_level_selector(std::size_t const index)
        -> selector<audio::mixer::stereo_level>
{
    return selector<audio::mixer::stereo_level>(
            [index](audio_state const& st) -> audio::mixer::stereo_level {
                return index < st.mixer_state.inputs.size()
                               ? st.mixer_state.inputs[index].level
                               : audio::mixer::stereo_level{};
            });
}

auto
make_input_enabled_selector(std::size_t const index) -> selector<bool>
{
    return selector<bool>([index](audio_state const& st) -> bool {
        return index < st.mixer_state.inputs.size()
                       ? st.mixer_state.inputs[index].enabled
                       : false;
    });
}

const selector<float> select_output_gain([](audio_state const& st) {
    return st.mixer_state.output.gain;
});

const selector<float> select_output_balance([](audio_state const& st) {
    return st.mixer_state.output.balance;
});

const selector<audio::mixer::stereo_level> select_output_level(
        [](audio_state const& st) { return st.mixer_state.output.level; });

const selector<bool> select_output_enabled([](audio_state const& st) {
    return st.mixer_state.output.enabled;
});

const selector<std::size_t> select_xruns([](audio_state const& st) {
    return st.xruns;
});

const selector<float> select_cpu_load([](audio_state const& st) {
    return st.cpu_load;
});

} // namespace piejam::runtime::audio_state_selectors
