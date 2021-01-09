// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ranges>

namespace piejam::range
{

template <std::integral T>
constexpr auto
indices(T max) noexcept
{
    return std::views::iota(T{}, max);
}

} // namespace piejam::range
