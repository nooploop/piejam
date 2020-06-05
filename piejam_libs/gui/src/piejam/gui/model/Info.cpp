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
