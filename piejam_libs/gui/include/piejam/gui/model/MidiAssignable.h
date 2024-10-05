// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <piejam/runtime/midi_assignment_id.h>

namespace piejam::gui::model
{

class MidiAssignable final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(QString, assignment, setAssignment)

public:
    MidiAssignable(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::midi_assignment_id const&);

    Q_INVOKABLE void startLearn();
    Q_INVOKABLE void stopLearn();

private:
    void onSubscribe() override;

    runtime::midi_assignment_id m_assignment_id;
};

} // namespace piejam::gui::model
