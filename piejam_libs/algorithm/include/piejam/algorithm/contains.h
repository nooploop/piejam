// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <algorithm>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

namespace detail
{

// until C++23, similiar implementation as published on
// https://en.cppreference.com
struct contains_fn
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
    [[nodiscard]]
    constexpr auto
    operator()(I first, S last, T const& value, Proj proj = {}) const -> bool
    {
        return std::ranges::find(
                       std::move(first),
                       last,
                       value,
                       std::move(proj)) != last;
    }

    template <std::ranges::input_range R, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate<
                         std::ranges::equal_to,
                         std::projected<std::ranges::iterator_t<R>, Proj>,
                         T const*>
    [[nodiscard]]
    constexpr auto
    operator()(R const& r, T const& value, Proj proj = {}) const -> bool
    {
        return (*this)(
                std::ranges::begin(r),
                std::ranges::end(r),
                value,
                std::move(proj));
    }
};

} // namespace detail

inline constexpr detail::contains_fn contains;

namespace detail
{

struct contains_if_fn
{
    template <
            std::input_iterator I,
            std::sentinel_for<I> S,
            class Proj = std::identity,
            std::indirect_unary_predicate<std::projected<I, Proj>> Predicate>
    [[nodiscard]]
    constexpr auto
    operator()(I first, S last, Predicate pred, Proj proj = {}) const -> bool
    {
        return std::ranges::find_if(
                       std::move(first),
                       last,
                       std::move(pred),
                       std::move(proj)) != last;
    }

    template <
            std::ranges::input_range R,
            class Proj = std::identity,
            std::indirect_unary_predicate<
                    std::projected<std::ranges::iterator_t<R>, Proj>> Predicate>
    [[nodiscard]]
    constexpr auto
    operator()(R const& r, Predicate pred, Proj proj = {}) const -> bool
    {
        return (*this)(
                std::ranges::begin(r),
                std::ranges::end(r),
                std::move(pred),
                std::move(proj));
    }
};

} // namespace detail

inline constexpr detail::contains_if_fn contains_if;

} // namespace piejam::algorithm
