// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    observe(runtime::selectors::make_bus_type_selector(m_bus_id),
            [this](audio::bus_type const bus_type) {
                setMono(bus_type == audio::bus_type::mono);
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
