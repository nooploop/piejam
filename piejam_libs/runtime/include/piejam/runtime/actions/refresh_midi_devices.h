// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct refresh_midi_devices final
    : ui::cloneable_action<refresh_midi_devices, action>
{
    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
