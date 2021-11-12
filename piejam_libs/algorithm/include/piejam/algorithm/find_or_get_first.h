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
constexpr auto
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
