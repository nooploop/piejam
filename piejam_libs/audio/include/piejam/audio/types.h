// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>
#include <utility>

namespace piejam::audio
{

enum class bus_type : bool
{
    mono,
    stereo
};

template <class T>
[[nodiscard]] constexpr auto
bus_type_to(bus_type const b, T&& mono_value, T&& stereo_value)
        -> decltype(auto)
{
    switch (b)
    {
        case bus_type::mono:
            return std::forward<T>(mono_value);
        case bus_type::stereo:
            return std::forward<T>(stereo_value);
    }
}

[[nodiscard]] constexpr auto
num_channels(bus_type const b) -> std::size_t
{
    return bus_type_to(b, std::size_t{1}, std::size_t{2});
}

enum class bus_channel
{
    mono,
    left,
    right
};

} // namespace piejam::audio
