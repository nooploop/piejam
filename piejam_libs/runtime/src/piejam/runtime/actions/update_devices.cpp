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

static constexpr void
update_channel(std::size_t& ch, std::size_t const num_chs)
{
    if (ch >= num_chs)
        ch = npos;
}

auto
update_devices::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.pcm_devices = pcm_devices;
    new_st.input = input;
    new_st.output = output;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    new_st.mixer_state.buses = [this](mixer::buses_t buses,
                                      mixer::bus_list_t const& in_ids,
                                      mixer::bus_list_t const& out_ids) {
        std::size_t const num_in_channels = input.hw_params->num_channels;
        for (auto const& in_id : in_ids)
        {
            auto& in = buses[in_id];
            update_channel(in.device_channels.left, num_in_channels);
            update_channel(in.device_channels.right, num_in_channels);
        }

        std::size_t const num_out_channels = output.hw_params->num_channels;
        for (auto const& out_id : out_ids)
        {
            auto& out = buses[out_id];
            update_channel(out.device_channels.left, num_out_channels);
            update_channel(out.device_channels.right, num_out_channels);
        }

        return buses;
    }(new_st.mixer_state.buses, st.mixer_state.inputs, st.mixer_state.outputs);

    return new_st;
}

} // namespace piejam::runtime::actions
