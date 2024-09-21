// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>

namespace piejam
{

namespace detail
{

template <class LeftCompare, class RightCompare>
struct interval_check
{
    template <class T>
    constexpr auto
    operator()(T const v, T const lo, T const hi) const noexcept -> bool
    {
        return LeftCompare{}(lo, v) && RightCompare{}(v, hi);
    }

    template <class T>
    constexpr auto operator()(T const v) const noexcept
    {
        return [this, v](T const lo, T const hi) { return (*this)(v, lo, hi); };
    }
};

} // namespace detail

inline constexpr auto const in_closed =
        detail::interval_check<std::less_equal<>, std::less_equal<>>{};

inline constexpr auto const in_open =
        detail::interval_check<std::less<>, std::less<>>{};

inline constexpr auto const in_left_open =
        detail::interval_check<std::less<>, std::less_equal<>>{};

inline constexpr auto const in_right_open =
        detail::interval_check<std::less_equal<>, std::less<>>{};

} // namespace piejam
