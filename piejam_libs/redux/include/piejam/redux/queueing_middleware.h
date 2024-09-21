// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/concepts.h>
#include <piejam/redux/flag_resetter.h>
#include <piejam/redux/functors.h>
#include <piejam/redux/middleware_functors.h>

#include <queue>

namespace piejam::redux
{

template <concepts::cloneable Action>
class queueing_middleware
{
public:
    template <class State>
    void
    operator()(middleware_functors<State, Action> const& mw_fs, Action const& a)
    {
        if (m_dispatching)
        {
            m_queued_actions.push(a.clone());
        }
        else
        {
            m_dispatching = true;
            flag_resetter reset_dispatching{&m_dispatching};

            mw_fs.next(a);

            while (!m_queued_actions.empty())
            {
                auto action = std::move(m_queued_actions.front());
                m_queued_actions.pop();
                mw_fs.next(*action);
            }
        }
    }

private:
    bool m_dispatching{};
    std::queue<std::unique_ptr<Action>> m_queued_actions;
};

} // namespace piejam::redux
