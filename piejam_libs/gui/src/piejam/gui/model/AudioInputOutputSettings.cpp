// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioInputOutputSettings.h>

#include <algorithm>
#include <cassert>

namespace piejam::gui::model
{

void
AudioInputOutputSettings::setChannels(QStringList const& channels)
{
    if (m_channels != channels)
    {
        m_channels = channels;
        emit channelsChanged();
    }
}

} // namespace piejam::gui::model
