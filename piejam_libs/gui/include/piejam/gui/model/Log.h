// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <QList>
#include <QStringList>

#include <memory>
#include <string>

namespace piejam::gui::model
{

class Log final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QStringList logMessages READ logMessages NOTIFY
                       logMessagesChanged FINAL)

public:
    Log(runtime::store_dispatch, runtime::subscriber&);

    auto logMessages() const -> QStringList
    {
        return m_logMessages;
    }

    void addLogMessage(QString const& msg)
    {
        m_logMessages.push_back(msg);
        emit logMessagesChanged();
    }

signals:

    void logMessagesChanged();

private:
    void onSubscribe() override;

    QStringList m_logMessages;
};

} // namespace piejam::gui::model
