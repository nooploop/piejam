// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/functors.h>

#include <cassert>
#include <utility>

namespace piejam::redux
{

template <class State, class Action>
class store
{
public:
    using reducer_f = redux::reducer_f<State, Action>;
    using subscriber_f = redux::subscriber_f<State>;
    using dispatch_f = redux::dispatch_f<Action>;
    using next_f = redux::next_f<Action>;
    using get_state_f = redux::get_state_f<State>;

    store(reducer_f reducer, State initial_state)
        : m_reducer(std::move(reducer))
        , m_state(std::move(initial_state))
    {
        assert(m_reducer);

        m_dispatch = [this](Action const& action) {
            m_state = m_reducer(m_state, action);

            if (m_subscriber)
                m_subscriber(m_state);
        };
    }

    auto state() const noexcept -> State const& { return m_state; }

    void dispatch(Action const& action) { m_dispatch(action); }

    template <class DispatchedAction>
    void dispatch(DispatchedAction&& action)
    {
        m_dispatch(std::forward<DispatchedAction>(action));
    }

    template <class MiddlewareFactory>
    void apply_middleware(MiddlewareFactory&& mf)
    {
        m_dispatch = std::invoke(
                std::forward<MiddlewareFactory>(mf),
                [this]() -> State const& { return m_state; }, // get_state
                [this](Action const& a) { dispatch(a); },     // dispatch
                std::move(m_dispatch));                       // next
    }

    void subscribe(subscriber_f s) { m_subscriber = std::move(s); }

private:
    reducer_f m_reducer;
    State m_state;

    next_f m_dispatch;

    subscriber_f m_subscriber;
};

} // namespace piejam::redux
