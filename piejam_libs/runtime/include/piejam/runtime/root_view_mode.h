// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::runtime
{

enum class root_view_mode : int
{
    mixer,
    info,
    settings,
    power,
    fx_browser,
    fx_module
};

} // namespace piejam::runtime
