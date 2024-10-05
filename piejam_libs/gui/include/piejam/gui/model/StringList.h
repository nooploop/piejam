// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>

#include <QObject>

namespace piejam::gui::model
{

class StringList final : public QObject
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(QStringList, elements, setElements)
    M_PIEJAM_GUI_PROPERTY(int, focused, setFocused)
};

} // namespace piejam::gui::model
