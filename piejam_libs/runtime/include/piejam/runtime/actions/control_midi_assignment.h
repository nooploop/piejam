// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct start_midi_learning final
    : ui::cloneable_action<start_midi_learning, reducible_action>
    , visitable_engine_action<start_midi_learning>
{
    midi_assignment_id assignment_id;

    [[nodiscard]] auto reduce(state const&) const -> state override;
};

struct stop_midi_learning final
    : ui::cloneable_action<stop_midi_learning, reducible_action>
    , visitable_engine_action<stop_midi_learning>
{
    [[nodiscard]] auto reduce(state const&) const -> state override;
};

struct update_midi_assignments final
    : ui::cloneable_action<update_midi_assignments, reducible_action>
    , visitable_engine_action<update_midi_assignments>
{
    midi_assignments_map assignments;

    [[nodiscard]] auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
