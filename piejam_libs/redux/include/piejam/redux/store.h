// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/concepts.h>
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

    template <concepts::reducer<State, Action> Reducer>
    store(Reducer&& reducer, State initial_state = {})
        : m_reducer(std::forward<Reducer>(reducer))
        , m_state(std::move(initial_state))
    {
        assert(m_reducer);

        m_dispatch = [this](Action const& action) {
            m_reducer(m_state, action);
            m_subscriber(m_state);
        };
    }

    [[nodiscard]] auto state() const noexcept -> State const&
    {
        return m_state;
    }

    void dispatch(Action const& action)
    {
        m_dispatch(action);
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

    template <concepts::subscriber<State> Subscriber>
    void subscribe(Subscriber&& s)
    {
        m_subscriber = std::forward<Subscriber>(s);
        assert(m_subscriber);
    }

private:
    reducer_f m_reducer;
    State m_state;

    next_f m_dispatch;

    subscriber_f m_subscriber{[](State const&) {}};
};

} // namespace piejam::redux
