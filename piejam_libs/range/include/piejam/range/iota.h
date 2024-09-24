// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ranges>

namespace piejam::range
{

template <std::integral T>
[[nodiscard]]
constexpr auto
iota(T const max) noexcept
{
    return std::views::iota(T{}, max);
}

} // namespace piejam::range
