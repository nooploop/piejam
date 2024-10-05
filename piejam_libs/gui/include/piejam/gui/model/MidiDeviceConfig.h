// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <piejam/midi/device_id.h>

namespace piejam::gui::model
{

class MidiDeviceConfig final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(QString, name, setName)
    M_PIEJAM_GUI_PROPERTY(bool, enabled, setEnabled)

public:
    MidiDeviceConfig(
            runtime::store_dispatch,
            runtime::subscriber&,
            midi::device_id_t);

    Q_INVOKABLE void changeEnabled(bool x);

private:
    void onSubscribe() override;

    midi::device_id_t m_device_id;
};

} // namespace piejam::gui::model
