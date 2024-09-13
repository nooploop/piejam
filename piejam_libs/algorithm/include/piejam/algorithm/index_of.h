// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>

#include <piejam/npos.h>

#include <cassert>
#include <functional>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

namespace detail
{

struct index_of_if_fn
{
    template <
            std::input_iterator I,
            std::sentinel_for<I> S,
            class Proj = std::identity,
            std::indirect_unary_predicate<std::projected<I, Proj>> Predicate>
    [[nodiscard]] constexpr auto
    operator()(I first, S last, Predicate pred, Proj proj = {}) const
            -> std::size_t
    {
        std::size_t pos{};
        while (first != last)
        {
            if (std::invoke(pred, std::invoke(proj, *first)))
            {
                return pos;
            }

            ++pos;
            ++first;
        }

        return npos;
    }

    template <
            std::ranges::input_range R,
            class Proj = std::identity,
            std::indirect_unary_predicate<
                    std::projected<std::ranges::iterator_t<R>, Proj>> Predicate>
    [[nodiscard]] constexpr auto
    operator()(R&& r, Predicate pred, Proj proj = {}) const -> std::size_t
    {
        return (*this)(
                std::ranges::begin(r),
                std::ranges::end(r),
                std::move(pred),
                std::move(proj));
    }
};

} // namespace detail

inline constexpr detail::index_of_if_fn index_of_if;

namespace detail
{

struct index_of_fn
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
    [[nodiscard]] constexpr auto
    operator()(I first, S last, T const& value, Proj proj = {}) const
            -> std::size_t
    {
        std::size_t pos{};
        while (first != last)
        {
            if (std::invoke(proj, *first) == value)
            {
                return pos;
            }

            ++pos;
            ++first;
        }

        return npos;
    }

    template <std::ranges::input_range R, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate<
                std::ranges::equal_to,
                std::projected<std::ranges::iterator_t<R>, Proj>,
                T const*>
    [[nodiscard]] constexpr auto
    operator()(R&& r, T const& value, Proj proj = {}) const -> std::size_t
    {
        return (*this)(
                std::ranges::begin(r),
                std::ranges::end(r),
                value,
                std::move(proj));
    }
};

} // namespace detail

inline constexpr detail::index_of_fn index_of;

} // namespace piejam::algorithm
