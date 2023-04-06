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

struct batch_action final : ui::cloneable_action<batch_action, action>
{
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return m_actions.empty();
    }

    [[nodiscard]] auto begin() const
    {
        return boost::make_indirect_iterator(m_actions.begin());
    }

    [[nodiscard]] auto end() const
    {
        return boost::make_indirect_iterator(m_actions.end());
    }

    void push_back(std::unique_ptr<action> action)
    {
        m_actions.push_back(std::move(action));
    }

    template <class Action, class... Args>
    void emplace_back(Args&&... args)
    {
        m_actions.push_back(
                std::make_unique<Action>(std::forward<Args>(args)...));
    }

    void append(batch_action other)
    {
        m_actions.insert(
                m_actions.end(),
                std::make_move_iterator(other.m_actions.begin()),
                std::make_move_iterator(other.m_actions.end()));
    }

private:
    std::vector<std::shared_ptr<action const>> m_actions;
};

inline auto
as_batch_action(action const& a) -> batch_action const*
{
    return dynamic_cast<batch_action const*>(&a);
}

} // namespace piejam::runtime::ui
