// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/functors.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/iterator/indirect_iterator.hpp>

#include <memory>
#include <vector>

namespace piejam::runtime::ui
{

template <class State>
struct batch_action final
    : ui::cloneable_action<batch_action<State>, action<State>>
{
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

    void append(batch_action<State> other)
    {
        m_actions.insert(
                m_actions.end(),
                std::make_move_iterator(other.m_actions.begin()),
                std::make_move_iterator(other.m_actions.end()));
    }

    auto reduce(State const&) const -> State override;

private:
    std::vector<std::shared_ptr<action<State> const>> m_actions;
};

template <class State>
auto
as_batch_action(action<State> const& a) -> batch_action<State> const*
{
    return dynamic_cast<batch_action<State> const*>(&a);
}

} // namespace piejam::runtime::ui
