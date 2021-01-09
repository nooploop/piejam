// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <vector>

namespace piejam::algorithm
{

template <class Range, class F>
auto
transform_to_vector(Range const& rng, F&& f)
{
    std::vector<std::decay_t<decltype(f(*std::begin(rng)))>> result;
    std::transform(
            std::begin(rng),
            std::end(rng),
            std::back_inserter(result),
            std::forward<F>(f));
    return result;
}

} // namespace piejam::algorithm
