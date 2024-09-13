// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/functional/in_interval.h>

namespace piejam
{

struct
{
    template <class T>
    constexpr auto operator()(T const& v, T const& l, T const& r) const noexcept
            -> bool
    {
        return in_right_open(v, l, r);
    }
} const rising_edge;

struct
{
    template <class T>
    constexpr auto operator()(T const& v, T const& l, T const& r) const noexcept
            -> bool
    {
        return in_left_open(v, r, l);
    }
} const falling_edge;

} // namespace piejam
