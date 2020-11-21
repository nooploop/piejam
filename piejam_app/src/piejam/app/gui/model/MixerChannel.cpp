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

#include <piejam/app/gui/model/MixerChannel.h>

#include <piejam/runtime/actions/set_bus_volume.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/audio_state_selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

MixerChannel::MixerChannel(
        store_dispatch store_dispatch,
        subscriber& state_change_subscriber,
        MixerChannelSelectors selectors,
        runtime::mixer::bus_id id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_selectors(std::move(selectors))
    , m_bus_id(id)
{
}

void
MixerChannel::subscribeStep(
        subscriber& state_change_subscriber,
        subscriptions_manager& subs,
        subscription_id subs_id)
{
    subs.observe(
            subs_id,
            state_change_subscriber,
            m_selectors.name,
            [this](container::boxed_string name) {
                setName(QString::fromStdString(*name));
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            runtime::audio_state_selectors::make_bus_volume_selector(m_bus_id),
            [this](float x) { setVolume(x); });

    subs.observe(
            subs_id,
            state_change_subscriber,
            m_selectors.pan,
            [this](float x) { setPanBalance(x); });

    subs.observe(
            subs_id,
            state_change_subscriber,
            m_selectors.mute,
            [this](bool x) { setMute(x); });

    subs.observe(
            subs_id,
            state_change_subscriber,
            m_selectors.solo,
            [this](bool x) { setSolo(x); });

    subs.observe(
            subs_id,
            state_change_subscriber,
            m_selectors.level,
            [this](runtime::mixer::stereo_level const& x) {
                setLevel(x.left, x.right);
            });
}

void
MixerChannel::changeVolume(double volume)
{
    dispatch(runtime::actions::set_bus_volume(
            m_bus_id,
            static_cast<float>(volume)));
}

} // namespace piejam::app::gui::model
