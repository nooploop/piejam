// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/refresh_devices.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
refresh_devices::reduce(state const& st) const -> state
{
    return st;
}

} // namespace piejam::runtime::actions
