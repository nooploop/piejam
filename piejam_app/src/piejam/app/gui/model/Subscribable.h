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

#include <piejam/app/subscriber.h>
#include <piejam/reselect/subscriptions_manager.h>

namespace piejam::app::gui::model
{

template <class Base>
class Subscribable : public Base
{
public:
    Subscribable(subscriber&);

    void subscribe() override final;
    void unsubscribe() override final;

protected:
    virtual void
    subscribeStep(subscriber&, subscriptions_manager&, subscription_id)
    {
    }

private:
    subscriber& m_state_change_subscriber;
    subscriptions_manager m_subs;
    bool m_subscribed{};
    subscription_id const m_subs_id{get_next_sub_id()};
};

template <class Base>
Subscribable<Base>::Subscribable(subscriber& state_change_subscriber)
    : m_state_change_subscriber(state_change_subscriber)
{
}

template <class Base>
void
Subscribable<Base>::subscribe()
{
    if (m_subscribed)
        return;

    subscribeStep(m_state_change_subscriber, m_subs, m_subs_id);

    m_subscribed = true;
}

template <class Base>
void
Subscribable<Base>::unsubscribe()
{
    if (!m_subscribed)
        return;

    m_subs.erase(m_subs_id);

    m_subscribed = false;
}

} // namespace piejam::app::gui::model
