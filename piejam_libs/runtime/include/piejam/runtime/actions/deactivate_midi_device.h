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

struct deactivate_midi_device final
    : ui::cloneable_action<deactivate_midi_device, reducible_action>
    , visitable_device_action<deactivate_midi_device>
{
    midi::device_id_t device_id;

    [[nodiscard]] auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
