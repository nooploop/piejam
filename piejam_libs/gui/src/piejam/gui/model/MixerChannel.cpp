// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannel.h>

namespace piejam::gui::model
{

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

void
MixerChannel::setSolo(bool const solo)
{
    if (m_solo != solo)
    {
        m_solo = solo;
        emit soloChanged();
    }
}

} // namespace piejam::gui::model
