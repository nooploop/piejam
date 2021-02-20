// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/mixer_actions.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
add_mixer_bus::reduce(state const& st) const -> state
{
    auto new_st = st;

    runtime::add_mixer_bus<io_direction::input>(new_st, {}, {});

    return new_st;
}

auto
delete_mixer_bus::reduce(state const& st) const -> state
{
    auto new_st = st;

    runtime::remove_mixer_bus(new_st, bus_id);

    return new_st;
}

auto
set_mixer_bus_name::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.buses.update(bus_id, [this](mixer::bus& bus) {
        bus.name = name;
    });

    return new_st;
}

} // namespace piejam::runtime::actions
