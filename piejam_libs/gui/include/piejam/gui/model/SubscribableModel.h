// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

    auto subscribed() const -> bool { return m_subscribed; }
    void setSubscribed(bool subs)
    {
        if (m_subscribed != subs)
        {
            if (subs)
                subscribe();
            else
                unsubscribe();

            m_subscribed = subs;

            emit subscribedChanged();
        }
    }

signals:
    void subscribedChanged();

protected:
    virtual void subscribe() = 0;
    virtual void unsubscribe() = 0;

private:
    bool m_subscribed{};
};

} // namespace piejam::gui::model
