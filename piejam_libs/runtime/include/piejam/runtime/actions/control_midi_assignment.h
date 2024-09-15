// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/audio_engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct start_midi_learning final
    : ui::cloneable_action<start_midi_learning, reducible_action>
    , visitable_audio_engine_action<start_midi_learning>
{
    midi_assignment_id assignment_id;

    void reduce(state&) const override;
};

struct stop_midi_learning final
    : ui::cloneable_action<stop_midi_learning, reducible_action>
    , visitable_audio_engine_action<stop_midi_learning>
{
    void reduce(state&) const override;
};

struct update_midi_assignments final
    : ui::cloneable_action<update_midi_assignments, reducible_action>
    , visitable_audio_engine_action<update_midi_assignments>
{
    midi_assignments_map assignments;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
