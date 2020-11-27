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

#include <piejam/indexed_access.h>

#include <QAbstractListModel>

#include <boost/assert.hpp>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

template <class ListItem>
class GenericListModel : public QAbstractListModel
{
public:
    enum Roles : int
    {
        ItemRole = Qt::UserRole
    };

    using QAbstractListModel::QAbstractListModel;

    auto rowCount(QModelIndex const& = QModelIndex()) const -> int override
    {
        return static_cast<int>(m_list.size());
    }

    auto data(QModelIndex const& index, int const role = Qt::DisplayRole) const
            -> QVariant override
    {
        switch (role)
        {
            case Qt::DisplayRole:
                return QVariant::fromValue(m_list[index.row()]->toQString());
            case ItemRole:
                return QVariant::fromValue(m_list[index.row()].get());
            default:
                return {};
        }
    }

    auto roleNames() const -> QHash<int, QByteArray> override
    {
        static QHash<int, QByteArray> s_roles = {
                {Qt::DisplayRole, "display"},
                {ItemRole, "item"}};
        return s_roles;
    }

    void add(std::size_t const pos, std::unique_ptr<ListItem> item)
    {
        BOOST_ASSERT(pos <= m_list.size());
        beginInsertRows(
                QModelIndex(),
                static_cast<int>(pos),
                static_cast<int>(pos));
        BOOST_ASSERT(item);
        insert_at(m_list, pos, std::move(item));
        endInsertRows();
    }

    void remove(std::size_t const pos)
    {
        BOOST_ASSERT(pos < m_list.size());
        beginRemoveRows(
                QModelIndex(),
                static_cast<int>(pos),
                static_cast<int>(pos));
        BOOST_ASSERT(!m_list.empty());
        erase_at(m_list, pos);
        endRemoveRows();
    }

    auto at(std::size_t pos) const -> ListItem const&
    {
        return *m_list.at(pos);
    }

private:
    std::vector<std::unique_ptr<ListItem>> m_list;
};

} // namespace piejam::gui::model
