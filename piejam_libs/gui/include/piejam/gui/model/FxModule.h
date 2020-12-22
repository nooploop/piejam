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

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxModule : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(QAbstractListModel* parameters READ parameters CONSTANT)
    Q_PROPERTY(
            bool canMoveLeft READ canMoveLeft NOTIFY canMoveLeftChanged FINAL)
    Q_PROPERTY(bool canMoveRight READ canMoveRight NOTIFY canMoveRightChanged
                       FINAL)

public:
    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    auto canMoveLeft() const noexcept -> bool { return m_canMoveLeft; }
    void setCanMoveLeft(bool x)
    {
        if (m_canMoveLeft != x)
        {
            m_canMoveLeft = x;
            emit canMoveLeftChanged();
        }
    }

    auto canMoveRight() const noexcept -> bool { return m_canMoveRight; }
    void setCanMoveRight(bool x)
    {
        if (m_canMoveRight != x)
        {
            m_canMoveRight = x;
            emit canMoveRightChanged();
        }
    }

    auto parameters() noexcept -> FxParametersList* { return &m_parameters; }

    Q_INVOKABLE virtual void deleteModule() = 0;
    Q_INVOKABLE virtual void moveLeft() = 0;
    Q_INVOKABLE virtual void moveRight() = 0;

    auto toQString() const -> QString { return m_name; }

signals:
    void nameChanged();
    void canMoveLeftChanged();
    void canMoveRightChanged();

private:
    QString m_name;
    FxParametersList m_parameters;
    bool m_canMoveLeft{};
    bool m_canMoveRight{};
};

} // namespace piejam::gui::model
