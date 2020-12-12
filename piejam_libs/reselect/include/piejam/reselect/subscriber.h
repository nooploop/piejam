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
    auto observe(selector<Value, State> sel, Handler&& h) -> subscription
    {
        std::invoke(std::forward<Handler>(h), sel.get(m_get_state()));
        auto token = std::make_shared<subscription::token>();
        return {m_observer.connect(
                        [sel = std::move(sel),
                         h = std::forward<Handler>(h),
                         token = std::weak_ptr(token),
                         last = std::make_shared<std::optional<Value>>()](
                                State const& st) mutable {
                            auto alive = token.lock();
                            if (!alive)
                                return;

                            auto current = sel.get(st);
                            if (!last->has_value() || **last != current)
                            {
                                h(current);
                                *last = current;
                            }
                        }),
                token};
    }

    void notify(State const& st) { m_observer(st); }

private:
    get_state_f m_get_state;
    nod::signal<void(State const&)> m_observer;
};

} // namespace piejam::reselect
