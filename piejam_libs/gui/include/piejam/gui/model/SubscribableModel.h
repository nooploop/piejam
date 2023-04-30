// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace piejam::gui::model
{

class SubscribableModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool subscribed READ subscribed WRITE setSubscribed NOTIFY
                       subscribedChanged)

public:
    using QObject::QObject;

    auto subscribed() const -> bool
    {
        return m_subscribed;
    }

    void setSubscribed(bool subs)
    {
        if (m_subscribed != subs)
        {
            if (subs)
            {
                subscribe();
            }
            else
            {
                unsubscribe();
            }

            m_subscribed = subs;

            emit subscribedChanged();
        }
    }

signals:
    void subscribedChanged();

protected:
    auto connectSubscribableChild(SubscribableModel& child)
    {
        return QObject::connect(
                this,
                &SubscribableModel::subscribedChanged,
                &child,
                [this, &child]() { child.setSubscribed(subscribed()); });
    }

    virtual void subscribe() = 0;
    virtual void unsubscribe() = 0;

private:
    bool m_subscribed{};
};

} // namespace piejam::gui::model
