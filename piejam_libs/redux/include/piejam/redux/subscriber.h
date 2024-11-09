// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/selector.h>
#include <piejam/redux/subscription.h>

#include <nod/nod.hpp>

#include <functional>
#include <memory>
#include <utility>

namespace piejam::redux
{

template <class State>
class subscriber
{
public:
    using get_state_f = std::function<State const&()>;

    template <std::invocable<> GetState>
    subscriber(GetState&& get_state)
        : m_get_state(std::forward<GetState>(get_state))
    {
    }

    template <class Value>
    [[nodiscard]]
    auto observe_once(selector<Value, State> const& sel) -> Value
    {
        return sel(m_get_state());
    }

    template <class Value, class Handler>
    [[nodiscard]]
    auto observe(selector<Value, State> sel, Handler&& handler) -> subscription
    {
        auto last = std::make_shared<Value>(sel(m_get_state()));
        std::invoke(std::forward<Handler>(handler), *last);
        auto token = std::make_shared<subscription::token>();
        return subscription(
                m_observer.connect([sel = std::move(sel),
                                    handler,
                                    token = std::weak_ptr(token),
                                    last](State const& st) mutable {
                    auto alive = token.lock();
                    if (!alive)
                    {
                        return;
                    }

                    auto current = sel(st);
                    if (*last != current)
                    {
                        handler(current);
                    }

                    *last = current;
                }),
                token);
    }

    void notify(State const& st)
    {
        m_observer(st);
    }

private:
    get_state_f m_get_state;
    nod::signal<void(State const&)> m_observer;
};

} // namespace piejam::redux
