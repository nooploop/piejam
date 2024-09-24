// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    [[nodiscard]]
    auto size() const -> std::size_t
    {
        return m_list.size();
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
        BOOST_ASSERT(pos < m_list.size());
        return *m_list[pos];
    }

private:
    std::vector<std::unique_ptr<ListItem>> m_list;
};

} // namespace piejam::gui::model
