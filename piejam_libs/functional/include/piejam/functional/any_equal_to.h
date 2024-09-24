// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <any>

namespace piejam
{

inline constexpr struct
{
    template <class T>
    [[nodiscard]]
    constexpr auto operator()(T const& t, std::any const& a) const -> bool
    {
        T const* const ta = std::any_cast<T>(&a);
        return ta && *ta == t;
    }
} any_equal_to;

inline constexpr struct
{
    template <class T>
    [[nodiscard]]
    constexpr auto operator()(T const& t, std::any const& a) const -> bool
    {
        T const* const ta = std::any_cast<T>(&a);
        return ta && *ta != t;
    }
} any_not_equal_to;

} // namespace piejam
