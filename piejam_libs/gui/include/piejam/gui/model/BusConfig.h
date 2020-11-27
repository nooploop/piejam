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

#include <piejam/gui/model/SubscribableModel.h>

namespace piejam::gui::model
{

class BusConfig : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool mono READ mono NOTIFY monoChanged FINAL)
    Q_PROPERTY(int monoChannel READ monoChannel NOTIFY monoChannelChanged FINAL)
    Q_PROPERTY(int stereoLeftChannel READ stereoLeftChannel NOTIFY
                       stereoLeftChannelChanged FINAL)
    Q_PROPERTY(int stereoRightChannel READ stereoRightChannel NOTIFY
                       stereoRightChannelChanged FINAL)

public:
    using SubscribableModel::SubscribableModel;

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const&);

    auto mono() const noexcept -> bool { return m_mono; }
    void setMono(bool);

    auto monoChannel() const noexcept -> unsigned { return m_monoChannel; }
    void setMonoChannel(unsigned);

    auto stereoLeftChannel() const noexcept -> unsigned
    {
        return m_stereoLeftChannel;
    }
    void setStereoLeftChannel(unsigned);

    auto stereoRightChannel() const noexcept -> unsigned
    {
        return m_stereoRightChannel;
    }
    void setStereoRightChannel(unsigned);

    auto toQString() const -> QString { return m_name; }

signals:

    void nameChanged();
    void monoChanged();
    void monoChannelChanged();
    void stereoLeftChannelChanged();
    void stereoRightChannelChanged();

private:
    QString m_name;
    bool m_mono{true};
    unsigned m_monoChannel{};
    unsigned m_stereoLeftChannel{};
    unsigned m_stereoRightChannel{};
};

} // namespace piejam::gui::model
