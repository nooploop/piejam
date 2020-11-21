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

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/app/subscriber.h>
#include <piejam/audio/types.h>
#include <piejam/container/boxed_string.h>
#include <piejam/entity_id.h>
#include <piejam/gui/model/BusConfig.h>

namespace piejam::app::gui::model
{

struct BusConfigSelectors
{
    selector<audio::bus_type> bus_type;
    selector<std::size_t> mono_channel;
    selector<std::size_t> stereo_left_channel;
    selector<std::size_t> stereo_right_channel;
};

class BusConfig final
    : public piejam::gui::model::BusConfig
    , public Subscribable
{
    Q_OBJECT

    Q_PROPERTY(bool subscribed READ subscribed WRITE setSubscribed NOTIFY
                       subscribedChanged)
public:
    BusConfig(
            store_dispatch,
            subscriber&,
            BusConfigSelectors,
            runtime::mixer::bus_id);

signals:
    void subscribedChanged();

private:
    void subscribeStep(subscriber&, subscriptions_manager&, subscription_id)
            override;

    void emitSubscribedChangedSignal() override { emit subscribedChanged(); }

    BusConfigSelectors m_selectors;
    runtime::mixer::bus_id m_bus_id;
};

} // namespace piejam::app::gui::model
