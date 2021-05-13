// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <QStringList>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class AudioInputOutputSettings : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged FINAL)
    Q_PROPERTY(QAbstractListModel* busConfigs READ busConfigs CONSTANT)

public:
    auto channels() -> QStringList { return m_channels; }
    void setChannels(QStringList const& channels)
    {
        if (m_channels != channels)
        {
            m_channels = channels;
            emit channelsChanged();
        }
    }

    auto busConfigs() -> BusConfigsList* { return &m_busConfigs; }

    Q_INVOKABLE virtual void addMonoBus() = 0;
    Q_INVOKABLE virtual void addStereoBus() = 0;

signals:
    void channelsChanged();

private:
    QStringList m_channels;
    BusConfigsList m_busConfigs;
};

} // namespace piejam::gui::model
