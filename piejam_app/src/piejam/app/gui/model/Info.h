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

#include <piejam/app/store.h>
#include <piejam/app/subscriber.h>
#include <piejam/gui/model/Info.h>
#include <piejam/reselect/subscriptions_manager.h>

namespace piejam::app::gui::model
{

class Info final : public piejam::gui::model::Info
{
public:
    Info(store&, subscriber&);

    void subscribe() override;
    void unsubscribe() override;

    void requestUpdate() override;

private:
    store& m_store;
    subscriber& m_state_change_subscriber;
    subscriptions_manager m_subs;
    bool m_subscribed{};
    subscription_id const m_subs_id{get_next_sub_id()};
};

} // namespace piejam::app::gui::model
