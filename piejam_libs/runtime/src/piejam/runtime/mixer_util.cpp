// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/mixer_util.h>

#include <piejam/entity_id.h>
#include <piejam/functional/overload.h>
#include <piejam/runtime/device_io.h>
#include <piejam/runtime/mixer.h>

namespace piejam::runtime
{

auto
mixer_bus_input_type(
        mixer::buses_t const& mixer_buses,
        mixer::bus_id const mixer_bus_id,
        device_io::buses_t const& device_buses) -> audio::bus_type
{
    mixer::bus const* const bus = mixer_buses.find(mixer_bus_id);
    if (!bus)
        return audio::bus_type::stereo;

    return std::visit(
            overload{
                    [&device_buses](device_io::bus_id const device_bus_id) {
                        return device_buses[device_bus_id].bus_type;
                    },
                    [](auto&&) { return audio::bus_type::stereo; }},
            bus->in);
}

} // namespace piejam::runtime
