// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>

namespace piejam::algorithm
{

template <class Range, class T>
auto
contains(Range const& rng, T&& value) -> bool
{
    return std::ranges::find(rng, std::forward<T>(value)) != std::end(rng);
}

} // namespace piejam::algorithm
