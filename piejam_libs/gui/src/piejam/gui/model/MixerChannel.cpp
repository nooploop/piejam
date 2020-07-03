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
MixerChannel::setEnabled(bool x)
{
    if (m_enabled != x)
    {
        m_enabled = x;
        emit enabledChanged();
    }
}

void
MixerChannel::setGain(double const x)
{
    if (m_gain != x)
    {
        m_gain = x;
        emit gainChanged();
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
MixerChannel::setPan(double const pan)
{
    if (!m_mono)
    {
        m_mono = true;
        emit monoChanged();
    }

    if (m_pan != pan)
    {
        m_pan = pan;
        emit panChanged();
    }
}

void
MixerChannel::setBalance(double const balance)
{
    if (m_mono)
    {
        m_mono = false;
        emit monoChanged();
    }

    if (m_balance != balance)
    {
        m_balance = balance;
        emit balanceChanged();
    }
}

} // namespace piejam::gui::model
