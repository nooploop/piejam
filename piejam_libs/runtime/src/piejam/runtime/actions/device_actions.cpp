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

#include <piejam/algorithm/npos.h>
#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

auto
refresh_devices::operator()(audio_state const& st) const -> audio_state
{
    return st;
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

    for (auto& in : new_st.mixer_state.inputs)
    {
        if (in.device_channel >= new_st.input.hw_params->num_channels)
            in.device_channel = algorithm::npos;
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
        new_st.mixer_state.inputs = piejam::audio::mixer::input_channels{
                new_st.input.hw_params->num_channels};

        std::size_t channel_index{};
        for (auto& in : new_st.mixer_state.inputs)
        {
            in.device_channel = channel_index++;
            in.name = "In " + std::to_string(in.device_channel + 1);
        }
    }
    else
    {
        auto const num_channels = new_st.output.hw_params->num_channels;
        new_st.mixer_state.outputs.clear();

        if (num_channels)
        {
            auto& out = new_st.mixer_state.outputs.emplace_back();
            out.name = std::string("Main");
            out.device_channels.left = num_channels > 0 ? 0 : algorithm::npos;
            out.device_channels.right = num_channels > 1 ? 1 : algorithm::npos;
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

auto
select_bus_channel::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    switch (direction)
    {
        case audio::bus_direction::input:
            assert(bus < new_st.mixer_state.inputs.size());
            assert(channel_selector == audio::bus_channel::mono);
            assert(channel_index == algorithm::npos ||
                   channel_index < new_st.input.hw_params->num_channels);
            new_st.mixer_state.inputs[bus].device_channel = channel_index;
            break;

        case audio::bus_direction::output:
            assert(bus == 0);
            assert(channel_selector == audio::bus_channel::left ||
                   channel_selector == audio::bus_channel::right);
            assert(channel_index == algorithm::npos ||
                   channel_index < new_st.output.hw_params->num_channels);
            break;

        default:
            assert(false);
            break;
    }

    return new_st;
}

auto
add_device_bus::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto& bus = new_st.mixer_state.inputs.emplace_back();
    bus.name = "In " + std::to_string(new_st.mixer_state.inputs.size());
    bus.device_channel = algorithm::npos;

    return new_st;
}

auto
delete_device_bus::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(bus < new_st.mixer_state.inputs.size());
    new_st.mixer_state.inputs.erase(new_st.mixer_state.inputs.begin() + bus);

    return new_st;
}

} // namespace piejam::runtime::actions
