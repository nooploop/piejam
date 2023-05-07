// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/midi/device_id.h>

namespace piejam::gui::model
{

class MidiDeviceConfig final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged FINAL)

public:
    MidiDeviceConfig(
            runtime::store_dispatch,
            runtime::subscriber&,
            midi::device_id_t);

    auto name() const noexcept -> QString const&
    {
        return m_name;
    }

    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    auto enabled() const noexcept -> bool
    {
        return m_enabled;
    }

    void setEnabled(bool x)
    {
        if (m_enabled != x)
        {
            m_enabled = x;
            emit enabledChanged();
        }
    }

    Q_INVOKABLE void changeEnabled(bool x);

signals:

    void nameChanged();
    void enabledChanged();

private:
    void onSubscribe() override;

    midi::device_id_t m_device_id;

    QString m_name;
    bool m_enabled{};
};

} // namespace piejam::gui::model
