// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/add_bus.h>

#include <piejam/runtime/state.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::runtime::actions
{

namespace
{

auto
default_bus_name(state const& st, io_direction io_dir, audio::bus_type bus_type)
{
    switch (io_dir)
    {
        case io_direction::input:
            return fmt::format(
                    "In {} {}",
                    st.device_io_state.inputs->size() + 1,
                    bus_type == audio::bus_type::mono ? "M" : "S");

        case io_direction::output:
            return fmt::format(
                    "Speaker {}",
                    st.device_io_state.outputs->size() + 1);
    }
}

auto
default_channels(state const& st, io_direction io_dir, audio::bus_type bus_type)
{
    auto num_channels =
            io_dir == io_direction::input
                    ? st.selected_io_sound_card.in.hw_params->num_channels
                    : st.selected_io_sound_card.out.hw_params->num_channels;

    std::vector<bool> assigned_channels(num_channels);

    auto const& bus_list = io_dir == io_direction::input
                                   ? *st.device_io_state.inputs
                                   : *st.device_io_state.outputs;

    for (external_audio::bus_id const bus_id : bus_list)
    {
        external_audio::bus const* const bus =
                st.device_io_state.buses.find(bus_id);
        BOOST_ASSERT(bus);
        if (auto ch = bus->channels.left; ch != npos)
        {
            assigned_channels[ch] = true;
        }

        if (auto ch = bus->channels.right; ch != npos)
        {
            assigned_channels[ch] = true;
        }
    }

    auto it = std::ranges::find(assigned_channels, false);
    std::size_t ch = it != assigned_channels.end()
                             ? std::distance(assigned_channels.begin(), it)
                             : npos;

    channel_index_pair channels{ch};

    if (bus_type == audio::bus_type::stereo)
    {
        if (it != assigned_channels.end())
        {
            it = std::find(std::next(it), assigned_channels.end(), false);
        }

        channels.right = it != assigned_channels.end()
                                 ? std::distance(assigned_channels.begin(), it)
                                 : npos;
    }

    return channels;
}

} // namespace

void
add_bus::reduce(state& st) const
{
    BOOST_ASSERT(
            direction != io_direction::output ||
            type == audio::bus_type::stereo);

    auto added_bus_id = add_device_bus(
            st,
            default_bus_name(st, direction, type),
            direction,
            type,
            default_channels(st, direction, type));

    if (direction == io_direction::output)
    {
        st.mixer_state.channels.update(
                st.mixer_state.main,
                [added_bus_id](mixer::channel& mixer_channel) {
                    if (std::holds_alternative<default_t>(mixer_channel.out))
                    {
                        mixer_channel.out = added_bus_id;
                    }
                });
    }
}

} // namespace piejam::runtime::actions
