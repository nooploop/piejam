// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/subscriber.h>

#include <unordered_map>

namespace piejam::redux
{

template <class Id>
class subscriptions_manager
{
public:
    template <class State, class Value, class Handler>
    void
    observe(Id id,
            subscriber<State>& subscriber,
            selector<Value, State> sel,
            Handler&& h)
    {
        m_conns.emplace(
                id,
                subscriber.observe(std::move(sel), std::forward<Handler>(h)));
    }

    void erase(Id id)
    {
        m_conns.erase(id);
    }

private:
    std::unordered_multimap<Id, subscription> m_conns;
};

} // namespace piejam::redux
