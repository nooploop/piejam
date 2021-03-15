// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>

namespace piejam::algorithm
{

template <class Range, class T>
constexpr auto
contains(Range const& rng, T&& value) -> bool
{
    return std::ranges::find(rng, std::forward<T>(value)) != std::end(rng);
}

template <class Range, class Predicate>
constexpr auto
contains_if(Range const& rng, Predicate&& p) -> bool
{
    return std::ranges::find_if(rng, std::forward<Predicate>(p)) !=
           std::end(rng);
}

} // namespace piejam::algorithm
