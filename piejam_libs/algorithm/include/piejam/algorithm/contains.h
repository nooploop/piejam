// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

namespace detail
{

// until C++23, similiar implementation as published on
// https://en.cppreference.com
struct __contains_fn
{
    template <
            std::input_iterator I,
            std::sentinel_for<I> S,
            class T,
            class Proj = std::identity>
        requires std::indirect_binary_predicate<
                         std::ranges::equal_to,
                         std::projected<I, Proj>,
                         T const*>
    constexpr auto
    operator()(I first, S last, T const& value, Proj proj = {}) const -> bool
    {
        return std::ranges::find(std::move(first), last, value, proj) != last;
    }

    template <std::ranges::input_range R, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate<
                         std::ranges::equal_to,
                         std::projected<std::ranges::iterator_t<R>, Proj>,
                         T const*>
    constexpr auto operator()(R&& r, T const& value, Proj proj = {}) const
            -> bool
    {
        return (*this)(std::ranges::begin(r), std::ranges::end(r), value, proj);
    }
};

} // namespace detail

inline constexpr detail::__contains_fn contains{};

template <
        std::ranges::input_range Range,
        range_unary_predicate<Range> Predicate>
constexpr auto
contains_if(Range const& rng, Predicate&& p) -> bool
{
    return std::ranges::find_if(rng, std::forward<Predicate>(p)) !=
           std::ranges::end(rng);
}

} // namespace piejam::algorithm
