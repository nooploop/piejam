// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

template <class State>
auto
thunk_action<State>::reduce(State const&) const -> State
{
    throw std::runtime_error("thunk_action is not reducible.");
}

} // namespace piejam::runtime::ui
