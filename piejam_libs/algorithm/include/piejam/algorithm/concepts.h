// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2022  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <class F, class Range>
concept range_binary_predicate = std::indirect_binary_predicate<
        F,
        std::ranges::iterator_t<Range>,
        std::ranges::iterator_t<Range>>;

} // namespace piejam::algorithm
