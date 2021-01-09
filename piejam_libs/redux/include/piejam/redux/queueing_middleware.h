// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/cloneable.h>
#include <piejam/redux/functors.h>
#include <piejam/redux/make_middleware.h>

#include <memory>
#include <queue>

namespace piejam::redux
{

template <class Action>
requires cloneable<Action> class queueing_middleware
{
public:
    queueing_middleware(next_f<Action> next)
        : m_next(std::move(next))
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

            m_next(a);

            while (!m_queued_actions.empty())
            {
                auto action = std::move(m_queued_actions.front());
                m_queued_actions.pop();
                m_next(*action);
            }

            m_dispatching = false;
        }
    }

private:
    next_f<Action> m_next;

    bool m_dispatching{};
    std::queue<std::unique_ptr<Action>> m_queued_actions;
};

template <class Action>
queueing_middleware(next_f<Action>) -> queueing_middleware<Action>;

inline constexpr struct
{
    template <class GetState, class Dispatch, class Action>
    auto operator()(GetState&&, Dispatch&&, next_f<Action> next) const
    {
        return make_middleware<queueing_middleware<Action>>(std::move(next));
    }
} make_queueing_middleware;

} // namespace piejam::redux
