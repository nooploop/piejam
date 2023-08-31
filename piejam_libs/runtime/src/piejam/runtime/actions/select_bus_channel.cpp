// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/select_bus_channel.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
select_bus_channel::reduce(state& st) const
{
    st.device_io_state.buses.update(bus_id, [this](device_io::bus& bus) {
        switch (channel_selector)
        {
            case audio::bus_channel::mono:
                bus.channels = channel_index_pair{channel_index};
                break;

            case audio::bus_channel::left:
                bus.channels.left = channel_index;
                break;

            case audio::bus_channel::right:
                bus.channels.right = channel_index;
                break;
        }
    });
}

} // namespace piejam::runtime::actions
