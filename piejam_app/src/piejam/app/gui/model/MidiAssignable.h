// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/midi_device_id.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/runtime/midi_assignment_id.h>

namespace piejam::app::gui::model
{

class MidiAssignable final
    : public Subscribable<piejam::gui::model::MidiAssignable>
{
public:
    MidiAssignable(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::midi_assignment_id const&);

    void startLearn() override;
    void stopLearn() override;

private:
    void onSubscribe() override;

    runtime::midi_assignment_id m_assignment_id;
};

} // namespace piejam::app::gui::model
