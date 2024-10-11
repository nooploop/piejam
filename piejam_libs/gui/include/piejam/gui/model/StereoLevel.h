// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>

#include <QObject>

namespace piejam::gui::model
{

class StereoLevel : public QObject
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(double, levelLeft, setLevelLeft)
    M_PIEJAM_GUI_PROPERTY(double, levelRight, setLevelRight)
};

} // namespace piejam::gui::model
