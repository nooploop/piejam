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

#include <fmt/format.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
select_bus_channel::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto& channels = direction == audio::bus_direction::input
                             ? new_st.mixer_state.inputs
                             : new_st.mixer_state.outputs;

    BOOST_ASSERT(bus < channels.size());
    BOOST_ASSERT(
            channel_index == npos ||
            channel_index < (direction == audio::bus_direction::input
                                     ? new_st.input.hw_params->num_channels
                                     : new_st.output.hw_params->num_channels));

    auto& ch = new_st.mixer_state.channels[channels[bus]];
    switch (channel_selector)
    {
        case audio::bus_channel::mono:
            BOOST_ASSERT(direction == audio::bus_direction::input);
            ch.device_channels = channel_index_pair{channel_index};
            break;

        case audio::bus_channel::left:
            BOOST_ASSERT(
                    direction == audio::bus_direction::input ||
                    direction == audio::bus_direction::output);
            ch.device_channels.left = channel_index;
            break;

        case audio::bus_channel::right:
            BOOST_ASSERT(
                    direction == audio::bus_direction::input ||
                    direction == audio::bus_direction::output);
            ch.device_channels.right = channel_index;
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
        new_st.mixer_state.inputs.push_back(
                new_st.mixer_state.channels.add(mixer::channel{
                        .name = fmt::format(
                                "In {}",
                                new_st.mixer_state.inputs.size() + 1),
                        .type = type}));
    }
    else
    {
        BOOST_ASSERT(direction == audio::bus_direction::output);
        auto const old_size = new_st.mixer_state.outputs.size();
        BOOST_ASSERT(type == audio::bus_type::stereo);
        auto name = old_size == 0 ? std::string("Main")
                                  : fmt::format("Aux {}", old_size);
        new_st.mixer_state.outputs.push_back(new_st.mixer_state.channels.add(
                mixer::channel{.name = std::move(name), .type = type}));
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
        BOOST_ASSERT(bus < inputs.size());
        auto const& id = inputs[bus];
        if (new_st.mixer_state.input_solo_id == id)
            new_st.mixer_state.input_solo_id = mixer::channel_id{};
        new_st.mixer_state.channels.remove(inputs[bus]);
        inputs.erase(inputs.begin() + bus);
    }
    else
    {
        auto& outputs = new_st.mixer_state.outputs;
        BOOST_ASSERT(direction == audio::bus_direction::output);
        BOOST_ASSERT(bus < outputs.size());
        new_st.mixer_state.channels.remove(outputs[bus]);
        outputs.erase(outputs.begin() + bus);
    }

    return new_st;
}

auto
set_input_channel_volume::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    mixer::input_channel(new_st.mixer_state, index).volume = volume;

    return new_st;
}

auto
set_input_channel_pan::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    mixer::input_channel(new_st.mixer_state, index).pan_balance = pan;

    return new_st;
}

auto
set_input_channel_mute::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    mixer::input_channel(new_st.mixer_state, index).mute = mute;

    return new_st;
}

auto
set_input_solo::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    BOOST_ASSERT(index < new_st.mixer_state.inputs.size());
    auto const& id = new_st.mixer_state.inputs[index];

    new_st.mixer_state.input_solo_id =
            id == new_st.mixer_state.input_solo_id ? mixer::channel_id{} : id;

    return new_st;
}

auto
set_output_channel_volume::operator()(audio_state const& st) const
        -> audio_state
{
    auto new_st = st;

    mixer::output_channel(new_st.mixer_state, index).volume = volume;

    return new_st;
}

auto
set_output_channel_balance::operator()(audio_state const& st) const
        -> audio_state
{
    auto new_st = st;

    mixer::output_channel(new_st.mixer_state, index).pan_balance = balance;

    return new_st;
}

auto
set_output_channel_mute::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    mixer::output_channel(new_st.mixer_state, index).mute = mute;

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

    auto const update =
            [](auto& channels_map, auto const& channels, auto const& levels) {
                std::size_t const num_levels = levels.size();
                BOOST_ASSERT(num_levels == channels.size());
                for (std::size_t index = 0; index < num_levels; ++index)
                    channels_map[channels[index]].level = levels[index];
            };

    update(new_st.mixer_state.channels, new_st.mixer_state.inputs, in_levels);
    update(new_st.mixer_state.channels, new_st.mixer_state.outputs, out_levels);

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
