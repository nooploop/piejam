// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/concepts.h>
#include <piejam/algorithm/npos.h>

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
    [[nodiscard]]
    constexpr auto operator()(I first, S last, Predicate pred, Proj proj = {})
            const -> std::size_t
    {
        for (std::size_t pos = 0; first != last; ++pos, ++first)
        {
            if (std::invoke(pred, std::invoke(proj, *first)))
            {
                return pos;
            }
        }

        return npos;
    }

    template <
            std::ranges::input_range R,
            class Proj = std::identity,
            std::indirect_unary_predicate<
                    std::projected<std::ranges::iterator_t<R>, Proj>> Predicate>
    [[nodiscard]]
    constexpr auto
    operator()(R const& r, Predicate pred, Proj proj = {}) const -> std::size_t
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
    [[nodiscard]]
    constexpr auto
    operator()(I first, S last, T value, Proj proj = {}) const -> std::size_t
    {
        for (std::size_t pos = 0; first != last; ++pos, ++first)
        {
            if (std::invoke(proj, *first) == value)
            {
                return pos;
            }
        }

        return npos;
    }

    template <std::ranges::input_range R, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate<
                         std::ranges::equal_to,
                         std::projected<std::ranges::iterator_t<R>, Proj>,
                         T const*>
    [[nodiscard]]
    constexpr auto
    operator()(R const& r, T value, Proj proj = {}) const -> std::size_t
    {
        return (*this)(
                std::ranges::begin(r),
                std::ranges::end(r),
                std::move(value),
                std::move(proj));
    }
};

} // namespace detail

inline constexpr detail::index_of_fn index_of;

} // namespace piejam::algorithm
