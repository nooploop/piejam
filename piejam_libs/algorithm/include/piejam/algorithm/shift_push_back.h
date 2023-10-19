// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <std::ranges::range Target, std::ranges::range Source>
constexpr auto
shift_push_back(Target&& target, Source&& source)
{
    using std::begin;
    using std::end;

    auto const size_source = std::ranges::size(source);
    auto const size_target = std::ranges::size(target);
    auto const count = std::min(size_source, size_target);
    std::shift_left(begin(target), end(target), count);
    std::copy(
            std::next(begin(source), size_source - count),
            end(source),
            std::next(begin(target), size_target - count));
}

} // namespace piejam::algorithm
