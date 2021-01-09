// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/npos.h>

#include <cassert>
#include <iterator>

namespace piejam::algorithm
{

template <class Range, class Predicate>
constexpr auto
index_of_if(Range const& rng, Predicate&& p)
{
    std::size_t pos{};
    for (auto const& x : rng)
    {
        if (std::forward<Predicate>(p)(x))
            return pos;
        ++pos;
    }

    return npos;
}

template <class Range, class T>
constexpr auto
index_of(Range const& rng, T&& value) -> std::size_t
{
    return index_of_if(rng, [v = std::forward<T>(value)](auto&& x) {
        return v == x;
    });
}

} // namespace piejam::algorithm
