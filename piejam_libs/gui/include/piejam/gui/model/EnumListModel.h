// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>

#include <boost/assert.hpp>

#include <utility>
#include <vector>

namespace piejam::gui::model
{

class EnumListModel : public QAbstractListModel
{
public:
    enum Roles : int
    {
        ValueRole = Qt::UserRole
    };

    using List = std::vector<std::pair<QString, int>>;

    explicit EnumListModel(List list)
        : m_list{std::move(list)}
    {
    }

    explicit EnumListModel(std::vector<std::pair<std::string, int>> const& list)
    {
        for (auto const& [text, value] : list)
        {
            m_list.emplace_back(QString::fromStdString(text), value);
        }
    }

    auto rowCount(QModelIndex const& = QModelIndex()) const -> int override
    {
        return static_cast<int>(m_list.size());
    }

    auto data(QModelIndex const& index, int const role = Qt::DisplayRole) const
            -> QVariant override
    {
        BOOST_ASSERT(index.row() < static_cast<int>(m_list.size()));

        switch (role)
        {
            case Qt::DisplayRole:
                return QVariant{m_list[index.row()].first};
            case ValueRole:
                return QVariant{m_list[index.row()].second};
            default:
                return {};
        }
    }

    auto roleNames() const -> QHash<int, QByteArray> override
    {
        static QHash<int, QByteArray> s_roles = {
                {Qt::DisplayRole, "text"},
                {ValueRole, "value"}};
        return s_roles;
    }

private:
    List m_list;
};

} // namespace piejam::gui::model
