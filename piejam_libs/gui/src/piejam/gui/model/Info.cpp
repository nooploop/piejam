// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Info.h>

#include <algorithm>
#include <cassert>

namespace piejam::gui::model
{

void
Info::setAudioLoad(double audioLoad)
{
    if (std::abs(m_audioLoad - audioLoad) > 1.e-3)
    {
        m_audioLoad = audioLoad;
        emit audioLoadChanged();
    }
}

void
Info::setXRuns(unsigned int const xruns)
{
    if (m_xruns != xruns)
    {
        m_xruns = xruns;
        emit xrunsChanged();
    }
}

void
Info::addLogMessage(QString const& msg)
{
    m_logData.push_back(msg);
    emit logDataChanged();
}

} // namespace piejam::gui::model
