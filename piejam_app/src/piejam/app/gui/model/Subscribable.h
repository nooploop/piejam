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

#include <piejam/entity_id_hash.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/store_dispatch.h>
#include <piejam/runtime/subscriber.h>

namespace piejam::app::gui::model
{

template <class Model>
class Subscribable : public Model
{
public:
    Subscribable(
            runtime::store_dispatch store_dispatch,
            runtime::subscriber& state_change_subscriber)
        : m_store_dispatch(store_dispatch)
        , m_state_change_subscriber(state_change_subscriber)
    {
    }

protected:
    auto state_change_subscriber() const noexcept -> runtime::subscriber&
    {
        return m_state_change_subscriber;
    }

    auto dispatch() const noexcept -> runtime::store_dispatch
    {
        return m_store_dispatch;
    }

    auto dispatch(runtime::action const& a) const { m_store_dispatch(a); }

    template <class Value, class Handler>
    void observe(runtime::selector<Value> sel, Handler&& h)
    {
        m_subs.observe(
                m_subs_id,
                m_state_change_subscriber,
                std::move(sel),
                std::forward<Handler>(h));
    }

    virtual void onSubscribe() {}

private:
    void subscribe() override { onSubscribe(); }
    void unsubscribe() override { m_subs.erase(m_subs_id); }

    runtime::store_dispatch m_store_dispatch;
    runtime::subscriber& m_state_change_subscriber;
    runtime::subscriptions_manager m_subs;
    bool m_subscribed{};
    runtime::subscription_id const m_subs_id{
            runtime::subscription_id::generate()};
};

} // namespace piejam::app::gui::model
