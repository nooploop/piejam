// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>

namespace piejam::runtime
{

// Material design colors
enum class material_color : std::uint8_t
{
    red,
    pink,
    purple,
    deep_purple,
    indigo,
    blue,
    light_blue,
    cyan,
    teal,
    green,
    light_green,
    lime,
    yellow,
    amber,
    orange,
    deep_orange,
    brown,
    grey,
    blue_grey,
};

} // namespace piejam::runtime
