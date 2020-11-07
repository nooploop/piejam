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

namespace piejam::app::gui::model
{

MixerChannel::MixerChannel(
        subscriber& state_change_subscriber,
        MixerChannelSelectors selectors)
    : base_t(state_change_subscriber)
    , m_selectors(std::move(selectors))
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
            m_selectors.volume,
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
            m_selectors.level,
            [this](runtime::mixer::stereo_level const& x) {
                setLevel(x.left, x.right);
            });
}

} // namespace piejam::app::gui::model
