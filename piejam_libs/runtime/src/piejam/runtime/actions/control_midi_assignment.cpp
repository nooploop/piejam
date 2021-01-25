// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/control_midi_assignment.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
start_midi_learning::reduce(state const& st) const -> state
{
    auto new_st = st;

    BOOST_ASSERT(is_valid_midi_assignment_id(assignment_id));
    new_st.midi_learning = assignment_id;

    return new_st;
}

auto
stop_midi_learning::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.midi_learning.reset();

    return new_st;
}

} // namespace piejam::runtime::actions
