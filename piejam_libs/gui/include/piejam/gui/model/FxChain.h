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
#include <piejam/gui/model/StringList.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxChain : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* modules READ modules CONSTANT)
    Q_PROPERTY(StringList* buses READ buses NOTIFY busesChanged FINAL)
    Q_PROPERTY(double levelLeft READ levelLeft NOTIFY levelLeftChanged FINAL)
    Q_PROPERTY(double levelRight READ levelRight NOTIFY levelRightChanged FINAL)
    Q_PROPERTY(double volume READ volume NOTIFY volumeChanged FINAL)

public:
    auto modules() -> FxModulesList* { return &m_modules; }
    auto buses() -> StringList* { return &m_buses; }

    Q_INVOKABLE virtual void selectBus(int) = 0;

    Q_INVOKABLE virtual void addModule() = 0;
    Q_INVOKABLE virtual void deleteModule(int) = 0;

    auto levelLeft() const noexcept -> double { return m_levelLeft; }
    auto levelRight() const noexcept -> double { return m_levelRight; }
    void setLevel(double left, double right)
    {
        if (m_levelLeft != left)
        {
            m_levelLeft = left;
            emit levelLeftChanged();
        }

        if (m_levelRight != right)
        {
            m_levelRight = right;
            emit levelRightChanged();
        }
    }
    Q_INVOKABLE virtual void requestLevelsUpdate() = 0;

    auto volume() const noexcept -> double { return m_volume; }
    void setVolume(double x)
    {
        if (m_volume != x)
        {
            m_volume = x;
            emit volumeChanged();
        }
    }
    Q_INVOKABLE virtual void changeVolume(double) = 0;

signals:
    void busesChanged();
    void selectedBusChanged();
    void levelLeftChanged();
    void levelRightChanged();
    void volumeChanged();

private:
    FxModulesList m_modules;
    StringList m_buses;
    double m_levelLeft{};
    double m_levelRight{};
    double m_volume{1.};
};

} // namespace piejam::gui::model
