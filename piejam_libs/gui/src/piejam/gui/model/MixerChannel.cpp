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

#include <piejam/gui/model/MixerChannel.h>

namespace piejam::gui::model
{

MixerChannel::MixerChannel(QObject* parent)
    : QObject(parent)
{
}

void
MixerChannel::setName(QString const& x)
{
    if (m_name != x)
    {
        m_name = x;
        emit nameChanged();
    }
}

void
MixerChannel::setVolume(double const x)
{
    if (m_volume != x)
    {
        m_volume = x;
        emit volumeChanged();
    }
}

void
MixerChannel::setLevel(double const left, double const right)
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

void
MixerChannel::setPanBalance(double const x)
{
    if (m_panBalance != x)
    {
        m_panBalance = x;
        emit panBalanceChanged();
    }
}

void
MixerChannel::setMute(bool const mute)
{
    if (m_mute != mute)
    {
        m_mute = mute;
        emit muteChanged();
    }
}

} // namespace piejam::gui::model
