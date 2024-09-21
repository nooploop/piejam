// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ranges>

namespace piejam::range
{

template <std::ranges::sized_range R>
[[nodiscard]] constexpr auto
indices(R&& rng) noexcept
{
    return std::views::iota(
            std::ranges::range_size_t<R>{},
            std::ranges::size(std::forward<R>(rng)));
}

} // namespace piejam::range
