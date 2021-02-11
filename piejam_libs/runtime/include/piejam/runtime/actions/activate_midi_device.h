// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/midi/device_id.h>
#include <piejam/runtime/actions/device_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct activate_midi_device final
    : ui::cloneable_action<activate_midi_device, action>
    , visitable_device_action<activate_midi_device>
{
    midi::device_id_t device_id;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
