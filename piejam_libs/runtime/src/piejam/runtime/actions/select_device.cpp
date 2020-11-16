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

#include <piejam/runtime/actions/select_device.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

#include <fmt/format.h>

namespace piejam::runtime::actions
{

template <>
auto
reduce_input_device(
        audio_state const&,
        select_device<audio::bus_direction::input> const& a) -> selected_device
{
    return a.device;
}

template <>
auto
reduce_input_device(
        audio_state const& st,
        select_device<audio::bus_direction::output> const&) -> selected_device
{
    return st.input;
}

template <>
auto
reduce_output_device(
        audio_state const& st,
        select_device<audio::bus_direction::input> const&) -> selected_device
{
    return st.output;
}

template <>
auto
reduce_output_device(
        audio_state const&,
        select_device<audio::bus_direction::output> const& a) -> selected_device
{
    return a.device;
}

template <audio::bus_direction D>
auto
reduce_samplerate(audio_state const&, select_device<D> const& a)
        -> audio::samplerate_t
{
    return a.samplerate;
}

template <audio::bus_direction D>
auto
reduce_period_size(audio_state const&, select_device<D> const& a)
        -> audio::period_size_t
{
    return a.period_size;
}

template <>
auto
reduce_mixer_state(
        audio_state const& st,
        select_device<audio::bus_direction::input> const& a) -> mixer::state
{
    auto mixer_state = st.mixer_state;

    mixer::clear_buses(mixer_state.buses, mixer_state.inputs);

    std::size_t const num_channels = a.device.hw_params->num_channels;
    for (std::size_t index = 0; index < num_channels; ++index)
    {
        mixer::add_bus(
                mixer_state.buses,
                mixer_state.inputs,
                mixer::bus{
                        .name = fmt::format("In {}", index + 1),
                        .type = audio::bus_type::mono,
                        .device_channels = channel_index_pair{index}});
    }

    return mixer_state;
}

template <>
auto
reduce_mixer_state(
        audio_state const& st,
        select_device<audio::bus_direction::output> const& a) -> mixer::state
{
    auto mixer_state = st.mixer_state;

    mixer::clear_buses(mixer_state.buses, mixer_state.outputs);

    if (auto const num_channels = a.device.hw_params->num_channels)
    {
        mixer::add_bus(
                mixer_state.buses,
                mixer_state.outputs,
                mixer::bus{
                        .name = std::string("Main"),
                        .type = audio::bus_type::stereo,
                        .device_channels = channel_index_pair(
                                num_channels > 0 ? 0 : npos,
                                num_channels > 1 ? 1 : npos)});
    }

    return mixer_state;
}

template auto
reduce(audio_state const&, select_device<audio::bus_direction::input> const&)
        -> audio_state;
template auto
reduce(audio_state const&, select_device<audio::bus_direction::output> const&)
        -> audio_state;

} // namespace piejam::runtime::actions
