// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/mixer_util.h>

#include <piejam/entity_id.h>
#include <piejam/runtime/device_io.h>
#include <piejam/runtime/mixer.h>

#include <boost/hof/match.hpp>

namespace piejam::runtime
{

auto
mixer_channel_input_type(
        mixer::channels_t const& channels,
        mixer::channel_id const channel_id,
        device_io::buses_t const& device_buses) -> audio::bus_type
{
    mixer::channel const* const mixer_channel = channels.find(channel_id);
    if (!mixer_channel)
        return audio::bus_type::stereo;

    return std::visit(
            boost::hof::match(
                    [&device_buses](device_io::bus_id const device_bus_id) {
                        return device_buses[device_bus_id].bus_type;
                    },
                    [](auto&&) { return audio::bus_type::stereo; }),
            mixer_channel->in);
}

} // namespace piejam::runtime
