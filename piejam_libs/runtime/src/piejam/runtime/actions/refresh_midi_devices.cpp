// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/refresh_midi_devices.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
refresh_midi_devices::reduce(state const& st) const -> state
{
    return st;
}

} // namespace piejam::runtime::actions
