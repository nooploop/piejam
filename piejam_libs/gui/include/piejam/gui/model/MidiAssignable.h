// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/runtime/midi_assignment_id.h>

namespace piejam::gui::model
{

class MidiAssignable final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(
            QString assignment READ assignment NOTIFY assignmentChanged FINAL)

public:
    MidiAssignable(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::midi_assignment_id const&);

    auto assignment() const -> QString
    {
        return m_assignment;
    }

    void setAssignment(QString const& x)
    {
        if (m_assignment != x)
        {
            m_assignment = x;
            emit assignmentChanged();
        }
    }

    Q_INVOKABLE void startLearn();
    Q_INVOKABLE void stopLearn();

signals:

    void assignmentChanged();

private:
    void onSubscribe() override;

    runtime::midi_assignment_id m_assignment_id;

    QString m_assignment;
};

} // namespace piejam::gui::model
