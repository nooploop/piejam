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

#include <piejam/algorithm/npos.h>
#include <piejam/container/boxify_result.h>
#include <piejam/functional/memo.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/audio_state.h>

#include <cassert>

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
        select_num_input_channels([](audio_state const& st) -> std::size_t {
            return st.input.hw_params->num_channels;
        });

const selector<std::size_t>
        select_num_input_busses([](audio_state const& st) -> std::size_t {
            return st.mixer_state.inputs.size();
        });

auto
make_bus_name_selector(audio::bus_direction const bd, std::size_t const bus)
        -> selector<container::boxed_string>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return selector<container::boxed_string>(
                    [bus](audio_state const& st) -> container::boxed_string {
                        assert(bus < st.mixer_state.inputs.size());
                        return st.mixer_state.inputs[bus].name;
                    });

        case audio::bus_direction::output:
            assert(bus == 0);
            return selector<container::boxed_string>(
                    [](audio_state const& st) -> container::boxed_string {
                        return st.mixer_state.output.name;
                    });
    }

    throw;
}

auto
make_bus_type_selector(audio::bus_direction const bd, std::size_t const bus)
        -> selector<audio::bus_type>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return selector<audio::bus_type>(
                    [](audio_state const & /*st*/) -> audio::bus_type {
                        return audio::bus_type::mono;
                    });

        case audio::bus_direction::output:
            assert(bus == 0);
            return selector<audio::bus_type>(
                    [](audio_state const & /*st*/) -> audio::bus_type {
                        return audio::bus_type::stereo;
                    });
    }

    throw;
}

auto
make_bus_channel_selector(
        audio::bus_direction const bd,
        std::size_t const bus,
        audio::bus_channel const bc) -> selector<std::size_t>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return selector<std::size_t>(
                    [bus](audio_state const& st) -> std::size_t {
                        assert(bus < st.mixer_state.inputs.size());
                        return st.mixer_state.inputs[bus].device_channel;
                    });

        case audio::bus_direction::output:
            assert(bus == 0);
            switch (bc)
            {
                case audio::bus_channel::left:
                    return selector<std::size_t>(
                            [](audio_state const& st) -> std::size_t {
                                return st.mixer_state.output.device_channels
                                        .left;
                            });

                case audio::bus_channel::right:
                    return selector<std::size_t>(
                            [](audio_state const& st) -> std::size_t {
                                return st.mixer_state.output.device_channels
                                        .right;
                            });

                default:
                    assert(false);
                    break;
            }
    }

    throw;
}

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

const selector<std::size_t>
        select_num_output_channels([](audio_state const& st) -> std::size_t {
            return st.output.hw_params->num_channels;
        });

const selector<std::size_t>
        select_num_output_busses([](audio_state const& st) -> std::size_t {
            return st.output.index == 0 ? 0 : 1;
        });

const selector<float> select_output_gain([](audio_state const& st) {
    return st.mixer_state.output.gain;
});

const selector<float> select_output_balance([](audio_state const& st) {
    return st.mixer_state.output.balance;
});

const selector<audio::mixer::stereo_level> select_output_level(
        [](audio_state const& st) { return st.mixer_state.output.level; });

const selector<std::size_t> select_xruns([](audio_state const& st) {
    return st.xruns;
});

const selector<float> select_cpu_load([](audio_state const& st) {
    return st.cpu_load;
});

} // namespace piejam::runtime::audio_state_selectors
