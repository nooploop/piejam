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
#include <piejam/audio/types.h>
#include <piejam/container/boxed_string.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/reselect/subscriptions_manager.h>

namespace piejam::app::gui::model
{

struct BusConfigSelectors
{
    reselect::selector<container::boxed_string, runtime::audio_state> name;
    reselect::selector<audio::bus_type, runtime::audio_state> bus_type;
    reselect::selector<std::size_t, runtime::audio_state> mono_channel;
    reselect::selector<std::size_t, runtime::audio_state> stereo_left_channel;
    reselect::selector<std::size_t, runtime::audio_state> stereo_right_channel;
};

class BusConfig final : public piejam::gui::model::BusConfig
{
public:
    BusConfig(subscriber&, BusConfigSelectors);

    void subscribe() override;
    void unsubscribe() override;

private:
    subscriber& m_state_change_subscriber;
    BusConfigSelectors m_selectors;
    bool m_subscribed{};
    subscription_id const m_subs_id{get_next_sub_id()};
    subscriptions_manager m_subs;
};

} // namespace piejam::app::gui::model
