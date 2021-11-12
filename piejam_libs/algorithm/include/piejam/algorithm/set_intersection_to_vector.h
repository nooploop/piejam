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

template <class Range1, class Range2>
auto
set_intersection_to_vector(Range1 const& r1, Range2 const& r2)
{
    using std::begin;
    using std::end;

    using r1_value_type = std::decay_t<decltype(*begin(r1))>;
    using r2_value_type = std::decay_t<decltype(*begin(r2))>;
    std::vector<std::common_type_t<r1_value_type, r2_value_type>> result;
    std::set_intersection(
            begin(r1),
            end(r1),
            begin(r2),
            end(r2),
            std::back_inserter(result));
    return result;
}

} // namespace piejam::algorithm
