// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/select_bus_channel.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
select_bus_channel::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.buses.update(bus_id, [this](mixer::bus& bus) {
        switch (channel_selector)
        {
            case audio::bus_channel::mono:
                bus.device_channels = channel_index_pair{channel_index};
                break;

            case audio::bus_channel::left:
                bus.device_channels.left = channel_index;
                break;

            case audio::bus_channel::right:
                bus.device_channels.right = channel_index;
                break;
        }
    });

    return new_st;
}

} // namespace piejam::runtime::actions
