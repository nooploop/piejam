// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/deactivate_midi_device.h>

#include <piejam/runtime/midi_device_config.h>
#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
deactivate_midi_device::reduce(state const& st) const -> state
{
    auto new_st = st;
    new_st.midi_devices.update([this](midi_devices_t& devices) {
        devices[device_id].enabled = false;
    });
    return new_st;
}

} // namespace piejam::runtime::actions
