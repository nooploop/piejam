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

#include <boost/ptr_container/ptr_vector.hpp>

namespace piejam::runtime::ui
{

template <class State>
struct batch_action final : cloneable_action<batch_action<State>, action<State>>
{
    bool empty() const noexcept { return m_actions.empty(); }
    auto begin() const { return m_actions.begin(); }
    auto end() const { return m_actions.end(); }

    void push_back(std::unique_ptr<action<State>> action)
    {
        m_actions.push_back(action.release());
    }

    template <class Action, class... Args>
    void emplace_back(Args&&... args)
    {
        m_actions.push_back(new Action{std::forward<Args>(args)...});
    }

    auto reduce(State const&) const -> State override;

private:
    boost::ptr_vector<action<State>> m_actions;
};

template <class State>
auto
new_clone(action<State> const& a) -> action<State>*
{
    return a.clone().release();
}

template <class State>
void
delete_clone(action<State> const* a)
{
    delete a;
}

template <class State>
auto
as_batch_action(action<State> const& a) -> batch_action<State> const*
{
    return dynamic_cast<batch_action<State> const*>(&a);
}

} // namespace piejam::runtime::ui
