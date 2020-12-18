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

#include <piejam/runtime/actions/select_fx_chain_bus.h>
#include <piejam/runtime/actions/set_bus_mute.h>
#include <piejam/runtime/actions/set_bus_pan_balance.h>
#include <piejam/runtime/actions/set_bus_volume.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

MixerChannel::MixerChannel(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::bus_id id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_bus_id(id)
{
}

void
MixerChannel::onSubscribe()
{
    observe(runtime::selectors::make_bus_name_selector(m_bus_id),
            [this](container::boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_bus_volume_selector(m_bus_id),
            [this](float x) { setVolume(x); });

    observe(runtime::selectors::make_bus_pan_balance_selector(m_bus_id),
            [this](float x) { setPanBalance(x); });

    observe(runtime::selectors::make_bus_mute_selector(m_bus_id),
            [this](bool x) { setMute(x); });

    observe(runtime::selectors::make_input_solo_selector(m_bus_id),
            [this](bool x) { setSolo(x); });

    observe(runtime::selectors::make_bus_level_selector(m_bus_id),
            [this](runtime::stereo_level const& x) {
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

void
MixerChannel::changePanBalance(double pan_balance)
{
    dispatch(runtime::actions::set_bus_pan_balance(
            m_bus_id,
            static_cast<float>(pan_balance)));
}

void
MixerChannel::changeMute(bool mute)
{
    dispatch(runtime::actions::set_bus_mute(m_bus_id, mute));
}

void
MixerChannel::focusFxChain()
{
    runtime::actions::select_fx_chain_bus action;
    action.bus_id = m_bus_id;
    dispatch(action);
}

} // namespace piejam::app::gui::model
