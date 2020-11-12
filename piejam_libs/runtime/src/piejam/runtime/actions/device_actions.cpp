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

namespace piejam::runtime::actions
{

auto
refresh_devices::operator()(audio_state const& st) const -> audio_state
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
update_devices::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    new_st.pcm_devices = pcm_devices;
    new_st.input = input;
    new_st.output = output;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    std::size_t const num_in_channels = new_st.input.hw_params->num_channels;
    for (auto& in : new_st.mixer_state.inputs)
    {
        update_channel(in.device_channels.left, num_in_channels);
        update_channel(in.device_channels.right, num_in_channels);
    }

    std::size_t const num_out_channels = new_st.output.hw_params->num_channels;
    for (auto& out : new_st.mixer_state.outputs)
    {
        update_channel(out.device_channels.left, num_out_channels);
        update_channel(out.device_channels.right, num_out_channels);
    }

    return new_st;
}

auto
initiate_device_selection::operator()(audio_state const& st) const
        -> audio_state
{
    return st;
}

auto
select_device::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    (input ? new_st.input : new_st.output) = device;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    if (input)
    {
        new_st.mixer_state.inputs =
                mixer::channels{new_st.input.hw_params->num_channels};

        std::size_t channel_index{};
        for (auto& in : new_st.mixer_state.inputs)
        {
            in.type = audio::bus_type::mono;
            in.device_channels = channel_index_pair{channel_index++};
            in.name = "In " + std::to_string(channel_index);
        }
    }
    else
    {
        auto const num_channels = new_st.output.hw_params->num_channels;
        new_st.mixer_state.outputs.clear();

        if (num_channels)
        {
            auto& out = new_st.mixer_state.outputs.emplace_back();
            out.type = audio::bus_type::stereo;
            out.name = std::string("Main");
            out.device_channels.left = num_channels > 0 ? 0 : npos;
            out.device_channels.right = num_channels > 1 ? 1 : npos;
        }
    }

    return new_st;
}

auto
select_samplerate::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const srs = samplerates_from_state(new_st);
    assert(index < srs.size());
    new_st.samplerate = srs[index];

    return new_st;
}

auto
select_period_size::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const pszs = period_sizes_from_state(new_st);
    assert(index < pszs.size());
    new_st.period_size = pszs[index];

    return new_st;
}

} // namespace piejam::runtime::actions
