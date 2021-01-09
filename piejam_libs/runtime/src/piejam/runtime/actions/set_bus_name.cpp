// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_bus_name.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
set_bus_name::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.buses.update(bus_id, [this](mixer::bus& bus) {
        bus.name = name;
    });

    return new_st;
}

} // namespace piejam::runtime::actions
