// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>

namespace piejam::algorithm
{

template <class Range, class T>
auto
find_or_get_first(Range const& rng, T&& value)
{
    auto first = std::begin(rng);
    auto last = std::end(rng);
    auto it = std::find(first, last, std::forward<T>(value));
    return it == last ? first : it;
}

} // namespace piejam::algorithm
