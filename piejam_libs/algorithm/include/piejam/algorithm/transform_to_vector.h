// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ranges>
#include <vector>

namespace piejam::algorithm
{

template <class F>
auto
transform_to_vector(std::ranges::range auto const& rng, F&& f)
{
    auto transformed = std::views::transform(rng, std::forward<F>(f));
    return std::vector(transformed.begin(), transformed.end());
}

} // namespace piejam::algorithm
