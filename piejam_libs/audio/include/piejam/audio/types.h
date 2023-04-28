// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace piejam::audio
{

enum class bus_type : bool
{
    mono,
    stereo
};

[[nodiscard]] constexpr auto
num_channels(bus_type const b) -> std::size_t
{
    switch (b)
    {
        case bus_type::mono:
            return 1;
        case bus_type::stereo:
            return 2;
    }
}

enum class bus_channel
{
    mono,
    left,
    right
};

} // namespace piejam::audio
