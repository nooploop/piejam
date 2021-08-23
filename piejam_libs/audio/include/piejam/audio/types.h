// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::audio
{

enum class bus_type : bool
{
    mono,
    stereo
};

enum class bus_channel
{
    mono,
    left,
    right
};

} // namespace piejam::audio
