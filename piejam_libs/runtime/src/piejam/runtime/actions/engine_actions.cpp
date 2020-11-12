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

#include <piejam/runtime/actions/engine_actions.h>

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

auto
select_bus_channel::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto& channels = direction == audio::bus_direction::input
                             ? new_st.mixer_state.inputs
                             : new_st.mixer_state.outputs;

    assert(bus < channels.size());
    assert(channel_index == npos ||
           channel_index < (direction == audio::bus_direction::input
                                    ? new_st.input.hw_params->num_channels
                                    : new_st.output.hw_params->num_channels));

    switch (channel_selector)
    {
        case audio::bus_channel::mono:
            assert(direction == audio::bus_direction::input);
            channels[bus].device_channels = channel_index_pair{channel_index};
            break;

        case audio::bus_channel::left:
            assert(direction == audio::bus_direction::input ||
                   direction == audio::bus_direction::output);
            channels[bus].device_channels.left = channel_index;
            break;

        case audio::bus_channel::right:
            assert(direction == audio::bus_direction::input ||
                   direction == audio::bus_direction::output);
            channels[bus].device_channels.right = channel_index;
            break;
    }

    return new_st;
}

auto
add_device_bus::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    if (direction == audio::bus_direction::input)
    {
        auto& bus = new_st.mixer_state.inputs.emplace_back();
        bus.type = type;
        bus.name = "In " + std::to_string(new_st.mixer_state.inputs.size());
    }
    else
    {
        assert(direction == audio::bus_direction::output);
        auto& bus = new_st.mixer_state.outputs.emplace_back();
        auto const new_size = new_st.mixer_state.outputs.size();
        assert(type == audio::bus_type::stereo);
        bus.type = type;
        bus.name = new_size == 1 ? std::string("Main")
                                 : "Aux " + std::to_string(new_size - 1);
    }

    return new_st;
}

auto
delete_device_bus::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    if (direction == audio::bus_direction::input)
    {
        auto& inputs = new_st.mixer_state.inputs;
        assert(bus < inputs.size());
        if (new_st.mixer_state.input_solo_index == bus)
            new_st.mixer_state.input_solo_index = npos;
        inputs.erase(inputs.begin() + bus);
    }
    else
    {
        auto& outputs = new_st.mixer_state.outputs;
        assert(direction == audio::bus_direction::output);
        assert(bus < outputs.size());
        outputs.erase(outputs.begin() + bus);
    }

    return new_st;
}

auto
set_input_channel_volume::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.inputs.size());
    auto& channel = new_st.mixer_state.inputs[index];
    channel.volume = volume;

    return new_st;
}

auto
set_input_channel_pan::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.inputs.size());
    auto& channel = new_st.mixer_state.inputs[index];
    channel.pan_balance = pan;

    return new_st;
}

auto
set_input_channel_mute::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.inputs.size());
    auto& channel = new_st.mixer_state.inputs[index];
    channel.mute = mute;

    return new_st;
}

auto
set_input_solo::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.inputs.size());

    new_st.mixer_state.input_solo_index =
            index == new_st.mixer_state.input_solo_index ? npos : index;

    return new_st;
}

auto
set_output_channel_volume::operator()(audio_state const& st) const
        -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.outputs.size());
    new_st.mixer_state.outputs[index].volume = volume;

    return new_st;
}

auto
set_output_channel_balance::operator()(audio_state const& st) const
        -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.outputs.size());
    new_st.mixer_state.outputs[index].pan_balance = balance;

    return new_st;
}

auto
set_output_channel_mute::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.outputs.size());
    new_st.mixer_state.outputs[index].mute = mute;

    return new_st;
}

auto
request_levels_update::operator()(audio_state const& st) const -> audio_state
{
    return st;
}

auto
update_levels::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const update = [](auto& channels, auto const& levels) {
        std::size_t const num_levels = levels.size();
        assert(num_levels == channels.size());
        for (std::size_t index = 0; index < num_levels; ++index)
            channels[index].level = levels[index];
    };

    update(new_st.mixer_state.inputs, in_levels);
    update(new_st.mixer_state.outputs, out_levels);

    return new_st;
}

auto
request_info_update::operator()(audio_state const& st) const -> audio_state
{
    return st;
}

auto
update_info::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    new_st.xruns = xruns;
    new_st.cpu_load = cpu_load;

    return new_st;
}

} // namespace piejam::runtime::actions
