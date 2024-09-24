// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>
#include <ranges>

namespace piejam::algorithm
{

template <class Result, std::ranges::sized_range... Range>
[[nodiscard]]
auto
concat(Range&&... rng) -> Result
{
    Result result;
    result.reserve((std::ranges::size(rng) + ...));
    (std::ranges::copy(rng, std::back_inserter(result)), ...);
    return result;
}

} // namespace piejam::algorithm
