// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/control_midi_assignment.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
start_midi_learning::reduce(state& st) const
{
    BOOST_ASSERT(is_valid_midi_assignment_id(assignment_id));
    st.midi_learning = assignment_id;
}

void
stop_midi_learning::reduce(state& st) const
{
    st.midi_learning.reset();
}

void
update_midi_assignments::reduce(state& st) const
{
    runtime::update_midi_assignments(st, assignments);
}

} // namespace piejam::runtime::actions
