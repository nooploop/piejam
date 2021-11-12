// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <std::ranges::forward_range Target, std::ranges::sized_range Source>
constexpr auto
shift_push_back(Target&& target, Source&& source)
{
    using std::begin;
    using std::end;
    using std::size;

    auto const size_source = size(source);
    auto const size_target = size(target);
    auto const count = std::min(size_source, size_target);
    std::shift_left(begin(target), end(target), count);
    std::copy(
            std::next(begin(source), size_source - count),
            end(source),
            std::next(begin(target), size_target - count));
}

} // namespace piejam::algorithm
