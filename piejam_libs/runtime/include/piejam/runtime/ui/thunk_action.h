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

#include <piejam/redux/functors.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <functional>

namespace piejam::runtime::ui
{

template <class State>
struct thunk_action final : cloneable_action<thunk_action<State>, action<State>>
{
    using get_state_f = redux::get_state_f<State>;
    using dispatch_f = redux::dispatch_f<action<State>>;

    template <class Thunk>
    thunk_action(Thunk&& thunk)
        : m_thunk(std::forward<Thunk>(thunk))
    {
    }

    void
    operator()(get_state_f const& get_state, dispatch_f const& dispatch) const
    {
        m_thunk(get_state, dispatch);
    }

    auto reduce(State const& st) const -> State override { return st; }

private:
    std::function<void(get_state_f const&, dispatch_f const&)> m_thunk;
};

template <class State>
auto
as_thunk_action(action<State> const& a) -> thunk_action<State> const*
{
    return dynamic_cast<thunk_action<State> const*>(&a);
}

} // namespace piejam::runtime::ui
