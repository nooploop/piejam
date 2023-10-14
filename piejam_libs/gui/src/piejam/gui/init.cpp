// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>

static void
initResources()
{
    Q_INIT_RESOURCE(piejam_gui_resources);
}

namespace piejam::gui
{

void
init()
{
    initResources();
}

} // namespace piejam::gui
