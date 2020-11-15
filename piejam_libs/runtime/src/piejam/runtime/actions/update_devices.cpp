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

#include <piejam/runtime/actions/update_devices.h>

#include <piejam/runtime/actions/reduce.h>
#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

template <>
auto
reduce_pcm_devices(audio_state const&, update_devices const& a)
        -> container::box<audio::pcm_io_descriptors>
{
    return a.pcm_devices;
}

template <>
auto
reduce_input_device(audio_state const&, update_devices const& a)
        -> selected_device
{
    return a.input;
}

template <>
auto
reduce_output_device(audio_state const&, update_devices const& a)
        -> selected_device
{
    return a.output;
}

template <>
auto
reduce_samplerate(audio_state const&, update_devices const& a)
        -> audio::samplerate_t
{
    return a.samplerate;
}

template <>
auto
reduce_period_size(audio_state const&, update_devices const& a)
        -> audio::period_size_t
{
    return a.period_size;
}

template <>
auto
reduce_mixer_state(audio_state const& st, update_devices const& a)
        -> mixer::state
{
    auto mixer_state = st.mixer_state;

    auto update_channel = [](std::size_t& ch, std::size_t const num_chs) {
        if (ch >= num_chs)
            ch = npos;
    };

    std::size_t const num_in_channels = a.input.hw_params->num_channels;
    for (auto const& in_id : mixer_state.inputs)
    {
        auto& in = mixer_state.buses[in_id];
        update_channel(in.device_channels.left, num_in_channels);
        update_channel(in.device_channels.right, num_in_channels);
    }

    std::size_t const num_out_channels = a.output.hw_params->num_channels;
    for (auto const& out_id : mixer_state.outputs)
    {
        auto& out = mixer_state.buses[out_id];
        update_channel(out.device_channels.left, num_out_channels);
        update_channel(out.device_channels.right, num_out_channels);
    }

    return mixer_state;
}

template auto reduce(audio_state const&, update_devices const&) -> audio_state;

} // namespace piejam::runtime::actions
