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

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

auto
update_devices::reduce(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto update_channel = [](std::size_t& ch, std::size_t const num_chs) {
        if (ch >= num_chs)
            ch = npos;
    };

    new_st.pcm_devices = pcm_devices;
    new_st.input = input;
    new_st.output = output;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    std::size_t const num_in_channels = input.hw_params->num_channels;
    for (auto const& in_id : new_st.mixer_state.inputs)
    {
        auto& in = new_st.mixer_state.buses[in_id];
        update_channel(in.device_channels.left, num_in_channels);
        update_channel(in.device_channels.right, num_in_channels);
    }

    std::size_t const num_out_channels = output.hw_params->num_channels;
    for (auto const& out_id : new_st.mixer_state.outputs)
    {
        auto& out = new_st.mixer_state.buses[out_id];
        update_channel(out.device_channels.left, num_out_channels);
        update_channel(out.device_channels.right, num_out_channels);
    }

    return new_st;
}

} // namespace piejam::runtime::actions
