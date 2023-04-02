// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2022  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <class F, class Range>
concept range_unary_predicate =
        std::indirect_unary_predicate<F, std::ranges::iterator_t<Range>>;

template <class F, class Range>
concept range_binary_predicate = std::indirect_binary_predicate<
        F,
        std::ranges::iterator_t<Range>,
        std::ranges::iterator_t<Range>>;

template <class Op, class Range>
concept range_binary_op = std::invocable<
        Op,
        std::ranges::range_value_t<Range>,
        std::ranges::range_value_t<Range>>;

template <class T, class Range>
concept convertible_to_range_value =
        std::convertible_to<T, std::ranges::range_value_t<Range>>;

template <class T, class Range>
concept equality_comparable_with_range_value =
        std::equality_comparable_with<T, std::ranges::range_reference_t<Range>>;

} // namespace piejam::algorithm
