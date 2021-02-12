// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/reselect/selector.h>
#include <piejam/reselect/subscription.h>

#include <nod/nod.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <utility>

namespace piejam::reselect
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

    template <class Value, class Handler>
    auto observe(selector<Value, State> sel, Handler&& handler) -> subscription
    {
        std::invoke(std::forward<Handler>(handler), sel.get(m_get_state()));
        auto token = std::make_shared<subscription::token>();
        return subscription{
                .m_conn = m_observer.connect(
                        [sel = std::move(sel),
                         handler,
                         token = std::weak_ptr(token),
                         last = std::make_shared<std::optional<Value>>()](
                                State const& st) mutable {
                            auto alive = token.lock();
                            if (!alive)
                                return;

                            auto current = sel.get(st);
                            if (!last->has_value() || **last != current)
                            {
                                handler(current);
                                *last = current;
                            }
                        }),
                .token = token};
    }

    void notify(State const& st) { m_observer(st); }

private:
    get_state_f m_get_state;
    nod::signal<void(State const&)> m_observer;
};

} // namespace piejam::reselect
