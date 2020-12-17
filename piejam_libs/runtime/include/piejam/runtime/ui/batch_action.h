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

#include <boost/iterator/indirect_iterator.hpp>

#include <memory>
#include <vector>

namespace piejam::runtime::ui
{

template <class State>
struct batch_action final : action<State>
{
    auto clone() const -> std::unique_ptr<action<State>> override
    {
        auto action = std::make_unique<batch_action<State>>();

        for (auto const& a : m_actions)
            action->push_back(a->clone());

        return action;
    }

    bool empty() const noexcept { return m_actions.empty(); }

    auto begin() const
    {
        return boost::make_indirect_iterator(m_actions.begin());
    }

    auto end() const { return boost::make_indirect_iterator(m_actions.end()); }

    void push_back(std::unique_ptr<action<State>> action)
    {
        m_actions.push_back(std::move(action));
    }

    template <class Action, class... Args>
    void emplace_back(Args&&... args)
    {
        m_actions.push_back(
                std::make_unique<Action>(std::forward<Args>(args)...));
    }

    auto reduce(State const&) const -> State override;

private:
    std::vector<std::unique_ptr<action<State>>> m_actions;
};

template <class State>
auto
as_batch_action(action<State> const& a) -> batch_action<State> const*
{
    return dynamic_cast<batch_action<State> const*>(&a);
}

} // namespace piejam::runtime::ui
