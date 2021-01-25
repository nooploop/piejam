// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct start_midi_learning final
    : ui::cloneable_action<start_midi_learning, action>
    , visitable_engine_action<start_midi_learning>
{
    midi_assignment_id assignment_id;

    auto reduce(state const&) const -> state override;
};

struct stop_midi_learning final
    : ui::cloneable_action<stop_midi_learning, action>
    , visitable_engine_action<stop_midi_learning>
{
    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
