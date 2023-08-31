// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/delete_bus.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
delete_bus::reduce(state& st) const
{
    remove_device_bus(st, bus_id);
}

} // namespace piejam::runtime::actions
