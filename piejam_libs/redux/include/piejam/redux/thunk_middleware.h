// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/functors.h>
#include <piejam/redux/make_middleware.h>

namespace piejam::redux
{

template <class State, class Action>
class thunk_middleware
{
public:
    thunk_middleware(
            get_state_f<State> get_state,
            dispatch_f<Action> dispatch,
            next_f<Action> next)
        : m_get_state(std::move(get_state))
        , m_dispatch(std::move(dispatch))
        , m_next(std::move(next))
    {
    }

    void operator()(Action const& a) const
    {
        if (auto* const ta = as_thunk_action(a))
        {
            (*ta)(m_get_state, m_dispatch);
        }
        else
        {
            m_next(a);
        }
    }

private:
    get_state_f<State> m_get_state;
    dispatch_f<Action> m_dispatch;
    next_f<Action> m_next;
};

template <class State, class Action>
struct make_thunk_middleware
{
    template <class GetState, class Dispatch, class Next>
    auto
    operator()(GetState&& get_state, Dispatch&& dispatch, Next&& next) const
    {
        return make_middleware<thunk_middleware<State, Action>>(
                std::forward<GetState>(get_state),
                std::forward<Dispatch>(dispatch),
                std::forward<Next>(next));
    }
};

} // namespace piejam::redux
