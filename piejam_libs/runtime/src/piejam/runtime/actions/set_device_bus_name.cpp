// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_device_bus_name.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
set_device_bus_name::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.device_io_state.buses.update(bus_id, [this](device_io::bus& bus) {
        bus.name = name;
    });

    new_st.mixer_state.buses.update(
            [this, addr_name = mixer::io_address_t(boxed_string(name))](
                    mixer::bus_id,
                    mixer::bus& bus) {
                if (bus.in == addr_name)
                    bus.in = bus_id;

                if (bus.out == addr_name)
                    bus.out = bus_id;
            });

    return new_st;
}

} // namespace piejam::runtime::actions
