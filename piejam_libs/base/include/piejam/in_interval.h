// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam
{

struct
{
    template <class T>
    constexpr auto
    operator()(T const& v, T const& lo, T const& hi) const noexcept -> bool
    {
        return lo <= v && v <= hi;
    }
} const in_closed;

struct
{
    template <class T>
    constexpr auto
    operator()(T const& v, T const& lo, T const& hi) const noexcept -> bool
    {
        return lo < v && v < hi;
    }
} const in_open;

struct
{
    template <class T>
    constexpr auto
    operator()(T const& v, T const& lo, T const& hi) const noexcept -> bool
    {
        return lo < v && v <= hi;
    }
} const in_left_open;

struct
{
    template <class T>
    constexpr auto
    operator()(T const& v, T const& lo, T const& hi) const noexcept -> bool
    {
        return lo <= v && v < hi;
    }
} const in_right_open;

} // namespace piejam
