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

class MixerChannel;

class MixerChannelsList : public QAbstractListModel
{
    Q_OBJECT

public:
    MixerChannelsList(QObject* parent = nullptr);
    ~MixerChannelsList();

    auto rowCount(const QModelIndex& parent = QModelIndex()) const
            -> int override;
    auto data(const QModelIndex& index, int role = Qt::DisplayRole) const
            -> QVariant override;

    auto roleNames() const -> QHash<int, QByteArray> override;

    void addMixerChannel(std::size_t pos, std::unique_ptr<MixerChannel>);
    void removeMixerChannel(std::size_t pos);

private:
    enum Roles : int
    {
        ItemRole = Qt::UserRole
    };

    std::vector<std::unique_ptr<MixerChannel>> m_list;
};

} // namespace piejam::gui::model
