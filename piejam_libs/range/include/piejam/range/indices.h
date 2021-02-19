// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ranges>

namespace piejam::range
{

template <std::ranges::sized_range R>
constexpr auto
indices(R&& rng) noexcept
{
    return std::views::iota(
            typename std::decay_t<R>::size_type{},
            std::ranges::size(std::forward<R>(rng)));
}

} // namespace piejam::range
