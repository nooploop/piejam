// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/concepts.h>
#include <piejam/redux/flag_resetter.h>
#include <piejam/redux/functors.h>

#include <queue>

namespace piejam::redux
{

template <concepts::cloneable Action>
class queueing_middleware
{
public:
    template <concepts::next<Action> Next>
    queueing_middleware(Next&& next)
        : m_next{std::forward<Next>(next)}
    {
    }

    void operator()(Action const& a)
    {
        if (m_dispatching)
        {
            m_queued_actions.push(a.clone());
        }
        else
        {
            m_dispatching = true;
            flag_resetter reset_dispatching{&m_dispatching};

            m_next(a);

            while (!m_queued_actions.empty())
            {
                auto action = std::move(m_queued_actions.front());
                m_queued_actions.pop();
                m_next(*action);
            }
        }
    }

private:
    next_f<Action> m_next;

    bool m_dispatching{};
    std::queue<std::unique_ptr<Action>> m_queued_actions;
};

} // namespace piejam::redux
