// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>

namespace piejam::algorithm
{

template <class Target, class Source>
auto
shift_push_back(Target&& target, Source&& source)
{
    auto const count = std::min(std::size(target), std::size(source));
    std::shift_left(std::begin(target), std::end(target), count);
    std::copy(
            std::next(std::begin(source), std::size(source) - count),
            source.end(),
            std::next(std::begin(target), std::size(target) - count));
}

} // namespace piejam::algorithm
