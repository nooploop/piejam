// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/MidiDeviceConfig.h>
#include <piejam/midi/device_id.h>

namespace piejam::app::gui::model
{

class MidiDeviceConfig final
    : public Subscribable<piejam::gui::model::MidiDeviceConfig>
{
public:
    MidiDeviceConfig(
            runtime::store_dispatch,
            runtime::subscriber&,
            midi::device_id_t device_id);

    void changeEnabled(bool x) override;

private:
    void onSubscribe() override;

    midi::device_id_t m_device_id;
};

} // namespace piejam::app::gui::model
