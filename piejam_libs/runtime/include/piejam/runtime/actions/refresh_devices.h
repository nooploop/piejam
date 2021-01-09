// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/device_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct refresh_devices final
    : ui::cloneable_action<refresh_devices, action>
    , visitable_device_action<refresh_devices>
{
    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
