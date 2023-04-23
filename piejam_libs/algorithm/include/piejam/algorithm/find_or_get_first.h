// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <std::ranges::input_range Range, convertible_to_range_value<Range> T>
[[nodiscard]] constexpr auto
find_or_get_first(Range const& rng, T&& value)
{
    using std::begin;
    using std::end;

    auto first = begin(rng);
    auto last = end(rng);
    auto it = std::find(first, last, std::forward<T>(value));
    return it == last ? first : it;
}

} // namespace piejam::algorithm
