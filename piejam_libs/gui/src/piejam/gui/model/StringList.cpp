// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StringList.h>

namespace piejam::gui::model
{

StringList::StringList(QObject* const parent)
    : QObject(parent)
{
}

void
StringList::setElements(QStringList const& x)
{
    if (m_elements != x)
    {
        m_elements = x;
        emit elementsChanged();
    }
}

void
StringList::setFocused(int const x)
{
    if (m_focused != x)
    {
        m_focused = x;
        emit focusedChanged();
    }
}

} // namespace piejam::gui::model
