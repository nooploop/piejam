// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/types.h>
#include <piejam/boxed_vector.h>
#include <piejam/gui/model/MidiInputSettings.h>
#include <piejam/midi/device_id.h>

namespace piejam::app::gui::model
{

class MidiInputSettings
    : public Subscribable<piejam::gui::model::MidiInputSettings>
{
public:
    MidiInputSettings(runtime::store_dispatch, runtime::subscriber&);

private:
    void onSubscribe() override;

    boxed_vector<midi::device_id_t> m_devices;
};

} // namespace piejam::app::gui::model
