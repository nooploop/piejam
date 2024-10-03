// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <algorithm>
#include <cassert>
#include <ranges>

namespace piejam::algorithm
{

template <std::ranges::input_range Range, convertible_to_range_value<Range> T>
[[nodiscard]]
constexpr auto
find_or_get_first(Range&& rng, T const& value)
{
    auto it = std::ranges::find(std::forward<Range>(rng), value);
    return it == std::ranges::end(rng) ? std::ranges::begin(rng) : it;
}

} // namespace piejam::algorithm
