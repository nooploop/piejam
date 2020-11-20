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

#pragma once

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <class Action>
auto
reduce_pcm_devices(audio_state const& st, Action const&)
        -> container::box<audio::pcm_io_descriptors>
{
    return st.pcm_devices;
}

template <class Action>
auto
reduce_input_device(audio_state const& st, Action const&) -> selected_device
{
    return st.input;
}

template <class Action>
auto
reduce_output_device(audio_state const& st, Action const&) -> selected_device
{
    return st.output;
}

template <class Action>
auto
reduce_samplerate(audio_state const& st, Action const&) -> audio::samplerate_t
{
    return st.samplerate;
}

template <class Action>
auto
reduce_period_size(audio_state const& st, Action const&) -> audio::period_size_t
{
    return st.period_size;
}

template <class Action>
auto
reduce_float_params(audio_state const& st, Action const&) -> float_parameters
{
    return st.float_params;
}

template <class Action>
auto
reduce_bool_params(audio_state const& st, Action const&) -> bool_parameters
{
    return st.bool_params;
}

template <class Action>
auto
reduce_mixer_state(audio_state const& st, Action const&) -> mixer::state
{
    return st.mixer_state;
}

template <class Action>
auto
reduce_xruns(audio_state const& st, Action const&) -> std::size_t
{
    return st.xruns;
}

template <class Action>
auto
reduce_cpu_load(audio_state const& st, Action const&) -> float
{
    return st.cpu_load;
}

template <class Action>
auto
reduce(audio_state const& st, Action const& a) -> audio_state
{
    return audio_state{
            .pcm_devices = reduce_pcm_devices(st, a),
            .input = reduce_input_device(st, a),
            .output = reduce_output_device(st, a),
            .samplerate = reduce_samplerate(st, a),
            .period_size = reduce_period_size(st, a),
            .float_params = reduce_float_params(st, a),
            .bool_params = reduce_bool_params(st, a),
            .mixer_state = reduce_mixer_state(st, a),
            .xruns = reduce_xruns(st, a),
            .cpu_load = reduce_cpu_load(st, a)};
}

} // namespace piejam::runtime::actions
