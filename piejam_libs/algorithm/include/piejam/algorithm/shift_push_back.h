// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <std::ranges::range Target, std::ranges::range Source>
constexpr auto
shift_push_back(Target&& target, Source const& source)
{
    auto const size_source = std::ranges::size(source);
    auto const size_target = std::ranges::size(target);
    auto const count = std::min(size_source, size_target);
    std::shift_left(
            std::ranges::begin(target),
            std::ranges::end(target),
            count);
    std::ranges::copy(
            std::ranges::next(std::ranges::begin(source), size_source - count),
            std::ranges::end(source),
            std::ranges::next(std::ranges::begin(target), size_target - count));
}

} // namespace piejam::algorithm
