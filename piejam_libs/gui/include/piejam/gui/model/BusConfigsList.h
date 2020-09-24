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

#include <QAbstractListModel>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class BusConfig;

class BusConfigsList : public QAbstractListModel
{
    Q_OBJECT

public:
    BusConfigsList(QObject* parent = nullptr);
    ~BusConfigsList();

    auto at(std::size_t index) -> BusConfig& { return *m_list[index]; }
    auto at(std::size_t index) const -> BusConfig const&
    {
        return *m_list[index];
    }

    auto rowCount(const QModelIndex& parent = QModelIndex()) const
            -> int override;
    auto data(const QModelIndex& index, int role = Qt::DisplayRole) const
            -> QVariant override;

    auto roleNames() const -> QHash<int, QByteArray> override;

    void addBusConfig(std::unique_ptr<BusConfig>);
    void removeBusConfig();

private:
    enum Roles : int
    {
        ItemRole = Qt::UserRole
    };

    std::vector<std::unique_ptr<BusConfig>> m_list;
};

} // namespace piejam::gui::model
