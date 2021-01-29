// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/MidiDeviceConfig.h>

#include <piejam/runtime/actions/activate_midi_device.h>
#include <piejam/runtime/actions/deactivate_midi_device.h>
#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

MidiDeviceConfig::MidiDeviceConfig(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        midi::device_id_t device_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_device_id(device_id)
{
}

void
MidiDeviceConfig::onSubscribe()
{
    observe(runtime::selectors::make_midi_device_name_selector(m_device_id),
            [this](std::string const& name) {
                setName(QString::fromStdString(name));
            });

    observe(runtime::selectors::make_midi_device_enabled_selector(m_device_id),
            [this](bool const x) { setEnabled(x); });
}

void
MidiDeviceConfig::changeEnabled(bool x)
{
    emit enabledChanged();

    if (x)
    {
        runtime::actions::activate_midi_device action;
        action.device_id = m_device_id;
        dispatch(action);
    }
    else
    {
        runtime::actions::deactivate_midi_device action;
        action.device_id = m_device_id;
        dispatch(action);
    }
}

} // namespace piejam::app::gui::model
