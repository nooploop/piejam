// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_device_bus_name.h>

#include <piejam/algorithm/contains.h>
#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
set_device_bus_name::reduce(state& st) const
{
    st.device_io_state.buses.update(bus_id, [this](device_io::bus& bus) {
        bus.name = name;
    });

    auto const io_dir = algorithm::contains(*st.device_io_state.inputs, bus_id)
                                ? io_direction::input
                                : io_direction::output;

    st.mixer_state.channels.update(
            [this,
             io_dir,
             route_name = mixer::io_address_t(mixer::missing_device_address(
                     name))](mixer::channel_id, mixer::channel& bus) {
                if (io_dir == io_direction::input)
                {
                    if (bus.in == route_name)
                    {
                        bus.in = bus_id;
                    }
                }
                else
                {
                    if (bus.out == route_name)
                    {
                        bus.out = bus_id;
                    }
                }
            });
}

} // namespace piejam::runtime::actions
