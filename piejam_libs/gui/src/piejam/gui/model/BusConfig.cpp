// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/BusConfig.h>

namespace piejam::gui::model
{

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
