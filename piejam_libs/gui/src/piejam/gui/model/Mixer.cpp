// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Mixer.h>

#include <algorithm>
#include <cassert>

namespace piejam::gui::model
{

void
Mixer::setInputSoloActive(bool const x)
{
    if (m_inputSoloActive != x)
    {
        m_inputSoloActive = x;
        emit inputSoloActiveChanged();
    }
}

} // namespace piejam::gui::model
