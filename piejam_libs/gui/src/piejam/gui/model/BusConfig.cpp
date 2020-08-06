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

#include <piejam/gui/model/BusConfig.h>

namespace piejam::gui::model
{

BusConfig::BusConfig(QObject* parent)
    : QObject(parent)
{
}

void
BusConfig::setName(QString const& x)
{
    if (m_name != x)
    {
        m_name = x;
        emit nameChanged();
    }
}

void
BusConfig::setMono(bool x)
{
    if (m_mono != x)
    {
        m_mono = x;
        emit monoChanged();
    }
}

void
BusConfig::setMonoChannel(unsigned x)
{
    if (m_monoChannel != x)
    {
        m_monoChannel = x;
        emit monoChannelChanged();
    }
}

void
BusConfig::setStereoLeftChannel(unsigned x)
{
    if (m_stereoLeftChannel != x)
    {
        m_stereoLeftChannel = x;
        emit stereoLeftChannelChanged();
    }
}

void
BusConfig::setStereoRightChannel(unsigned x)
{
    if (m_stereoRightChannel != x)
    {
        m_stereoRightChannel = x;
        emit stereoRightChannelChanged();
    }
}

} // namespace piejam::gui::model
