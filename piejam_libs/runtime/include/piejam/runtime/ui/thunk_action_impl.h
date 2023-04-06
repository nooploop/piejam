// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::runtime::ui
{

template <class State>
void
thunk_action<State>::operator()(
        get_state_f const& get_state,
        dispatch_f const& dispatch) const
{
    m_thunk(get_state, dispatch);
}

} // namespace piejam::runtime::ui
