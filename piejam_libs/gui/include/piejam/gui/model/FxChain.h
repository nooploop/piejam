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

class FxChain : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* modules READ modules CONSTANT)
    Q_PROPERTY(QAbstractListModel* buses READ buses CONSTANT)
    Q_PROPERTY(int selectedBus READ selectedBus NOTIFY selectedBusChanged FINAL)

public:
    auto modules() -> FxModulesList* { return &m_modules; }
    auto buses() -> BusNamesList* { return &m_buses; }

    auto selectedBus() const noexcept -> int { return m_selectedBus; }
    void setSelectedBus(int bus)
    {
        if (m_selectedBus != bus)
        {
            m_selectedBus = bus;
            emit selectedBusChanged();
        }
    }

    Q_INVOKABLE virtual void selectBus(int) = 0;

signals:
    void selectedBusChanged();

private:
    FxModulesList m_modules;
    BusNamesList m_buses;
    int m_selectedBus{-1};
};

} // namespace piejam::gui::model
