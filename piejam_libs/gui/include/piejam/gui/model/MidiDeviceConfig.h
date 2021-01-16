// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SubscribableModel.h>

namespace piejam::gui::model
{

class MidiDeviceConfig : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged FINAL)

public:
    using SubscribableModel::SubscribableModel;

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    auto enabled() const noexcept -> bool { return m_enabled; }
    void setEnabled(bool x)
    {
        if (m_enabled != x)
        {
            m_enabled = x;
            emit enabledChanged();
        }
    }

    Q_INVOKABLE virtual void changeEnabled(bool x) = 0;

signals:

    void nameChanged();
    void enabledChanged();

private:
    QString m_name;
    bool m_enabled{};
};

} // namespace piejam::gui::model
