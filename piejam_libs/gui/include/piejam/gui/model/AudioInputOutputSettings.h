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
#include <piejam/gui/model/fwd.h>

#include <QObject>
#include <QStringList>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class AudioInputOutputSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged FINAL)
    Q_PROPERTY(QAbstractListModel* busConfigs READ busConfigs CONSTANT)

public:
    auto channels() -> QStringList { return m_channels; }
    void setChannels(QStringList const& channels);

    auto busConfigs() -> BusConfigsList* { return &m_busConfigs; }

    Q_INVOKABLE virtual void setBusName(unsigned name, QString const&) = 0;
    Q_INVOKABLE virtual void selectMonoChannel(unsigned bus, unsigned ch) = 0;
    Q_INVOKABLE virtual void
    selectStereoLeftChannel(unsigned bus, unsigned ch) = 0;
    Q_INVOKABLE virtual void
    selectStereoRightChannel(unsigned bus, unsigned ch) = 0;
    Q_INVOKABLE virtual void addMonoBus() = 0;
    Q_INVOKABLE virtual void addStereoBus() = 0;
    Q_INVOKABLE virtual void deleteBus(unsigned bus) = 0;

signals:

    void channelsChanged();

private:
    QStringList m_channels;
    BusConfigsList m_busConfigs;
};

} // namespace piejam::gui::model
