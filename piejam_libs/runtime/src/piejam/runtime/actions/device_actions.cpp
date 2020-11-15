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

#include <piejam/runtime/actions/device_actions.h>

#include <piejam/npos.h>
#include <piejam/runtime/audio_state.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
refresh_devices::reduce(audio_state const& st) const -> audio_state
{
    return st;
}

static void
update_channel(std::size_t& cur_ch, std::size_t const num_chs)
{
    if (cur_ch >= num_chs)
        cur_ch = npos;
}

auto
update_devices::reduce(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    new_st.pcm_devices = pcm_devices;
    new_st.input = input;
    new_st.output = output;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    std::size_t const num_in_channels = new_st.input.hw_params->num_channels;
    for (auto const& in_id : new_st.mixer_state.inputs)
    {
        auto& in = new_st.mixer_state.buses[in_id];
        update_channel(in.device_channels.left, num_in_channels);
        update_channel(in.device_channels.right, num_in_channels);
    }

    std::size_t const num_out_channels = new_st.output.hw_params->num_channels;
    for (auto const& out_id : new_st.mixer_state.outputs)
    {
        auto& out = new_st.mixer_state.buses[out_id];
        update_channel(out.device_channels.left, num_out_channels);
        update_channel(out.device_channels.right, num_out_channels);
    }

    return new_st;
}

auto
initiate_device_selection::reduce(audio_state const& st) const -> audio_state
{
    return st;
}

auto
select_device::reduce(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    (input ? new_st.input : new_st.output) = device;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    if (input)
    {
        mixer::clear_buses(new_st.mixer_state.buses, new_st.mixer_state.inputs);

        std::size_t const num_channels = new_st.input.hw_params->num_channels;
        for (std::size_t index = 0; index < num_channels; ++index)
        {
            new_st.mixer_state.inputs.push_back(
                    new_st.mixer_state.buses.add(mixer::bus{
                            .name = fmt::format("In {}", index + 1),
                            .type = audio::bus_type::mono,
                            .device_channels = channel_index_pair{index}}));
        }
    }
    else
    {
        mixer::clear_buses(
                new_st.mixer_state.buses,
                new_st.mixer_state.outputs);

        auto const num_channels = new_st.output.hw_params->num_channels;
        if (num_channels)
        {
            new_st.mixer_state.outputs.push_back(
                    new_st.mixer_state.buses.add(mixer::bus{
                            .name = std::string("Main"),
                            .type = audio::bus_type::stereo,
                            .device_channels = channel_index_pair(
                                    num_channels > 0 ? 0 : npos,
                                    num_channels > 1 ? 1 : npos)}));
        }
    }

    return new_st;
}

auto
select_samplerate::reduce(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const srs = samplerates_from_state(new_st);
    BOOST_ASSERT(index < srs.size());
    new_st.samplerate = srs[index];

    return new_st;
}

auto
select_period_size::reduce(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const pszs = period_sizes_from_state(new_st);
    BOOST_ASSERT(index < pszs.size());
    new_st.period_size = pszs[index];

    return new_st;
}

} // namespace piejam::runtime::actions
