// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/concepts.h>
#include <piejam/redux/functors.h>

#include <boost/assert.hpp>

namespace piejam::redux
{

template <class State, class Action>
class middleware_functors
{
public:
    template <
            concepts::get_state<State> GetState,
            concepts::dispatch<Action> Dispatch,
            concepts::next<Action> Next>
    middleware_functors(GetState&& get_state, Dispatch&& dispatch, Next&& next)
        : m_get_state{std::forward<GetState>(get_state)}
        , m_dispatch{std::forward<Dispatch>(dispatch)}
        , m_next{std::forward<Next>(next)}
    {
    }

    auto get_state() const -> State const&
    {
        BOOST_ASSERT(m_get_state);
        return m_get_state();
    }

    auto get_state_f() const
    {
        return m_get_state;
    }

    void dispatch(Action const& a) const
    {
        BOOST_ASSERT(m_dispatch);
        m_dispatch(a);
    }

    auto dispatch_f() const
    {
        return m_dispatch;
    }

    void next(Action const& a) const
    {
        BOOST_ASSERT(m_next);
        m_next(a);
    }

private:
    redux::get_state_f<State> m_get_state;
    redux::dispatch_f<Action> m_dispatch;
    next_f<Action> m_next;
};

} // namespace piejam::redux
