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

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/select_fx_chain_bus.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

MixerChannel::MixerChannel(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::bus_id const id,
        runtime::float_parameter_id const volume,
        runtime::float_parameter_id const pan_balance,
        runtime::bool_parameter_id const mute,
        runtime::stereo_level_parameter_id const level)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_bus_id(id)
    , m_volume(volume)
    , m_pan_balance(pan_balance)
    , m_mute(mute)
    , m_level(level)
{
}

void
MixerChannel::onSubscribe()
{
    observe(runtime::selectors::make_bus_name_selector(m_bus_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_float_parameter_value_selector(m_volume),
            [this](float x) { setVolume(x); });

    observe(runtime::selectors::make_float_parameter_value_selector(
                    m_pan_balance),
            [this](float x) { setPanBalance(x); });

    observe(runtime::selectors::make_bool_parameter_value_selector(m_mute),
            [this](bool x) { setMute(x); });

    observe(runtime::selectors::make_input_solo_selector(m_bus_id),
            [this](bool x) { setSolo(x); });

    observe(runtime::selectors::make_level_parameter_value_selector(m_level),
            [this](runtime::stereo_level const& x) {
                setLevel(x.left, x.right);
            });
}

void
MixerChannel::changeVolume(double value)
{
    dispatch(runtime::actions::set_float_parameter(
            m_volume,
            static_cast<float>(value)));
}

void
MixerChannel::changePanBalance(double value)
{
    dispatch(runtime::actions::set_float_parameter(
            m_pan_balance,
            static_cast<float>(value)));
}

void
MixerChannel::changeMute(bool value)
{
    dispatch(runtime::actions::set_bool_parameter(m_mute, value));
}

void
MixerChannel::focusFxChain()
{
    runtime::actions::select_fx_chain_bus action;
    action.bus_id = m_bus_id;
    dispatch(action);
}

} // namespace piejam::app::gui::model
