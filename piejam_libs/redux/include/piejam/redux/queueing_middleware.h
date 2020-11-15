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
