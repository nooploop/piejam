// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/SubscribableModel.h>

namespace piejam::gui::model
{

class MidiAssignable : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(
            QString assignment READ assignment NOTIFY assignmentChanged FINAL)

public:
    auto assignment() const -> QString { return m_assignment; }
    void setAssignment(QString const& x)
    {
        if (m_assignment != x)
        {
            m_assignment = x;
            emit assignmentChanged();
        }
    }

    Q_INVOKABLE virtual void startLearn() = 0;
    Q_INVOKABLE virtual void stopLearn() = 0;

signals:

    void assignmentChanged();

private:
    QString m_assignment;
};

} // namespace piejam::gui::model
