// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    template <class Value>
    auto observe_once(runtime::selector<Value> const& sel)
    {
        return m_state_change_subscriber.observe_once(sel);
    }

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
