// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam {

template <class T>
constexpr bool in_closed(T const& v, T const& lo, T const& hi) noexcept
{
    return lo <= v && v <= hi;
}

template <class T>
constexpr bool in_open(T const& v, T const& lo, T const& hi) noexcept
{
    return lo < v && v < hi;
}

template <class T>
constexpr bool in_left_open(T const& v, T const& lo, T const& hi) noexcept
{
    return lo < v && v <= hi;
}

template <class T>
constexpr bool in_right_open(T const& v, T const& lo, T const& hi) noexcept
{
    return lo <= v && v < hi;
}

} // namespace piejam
