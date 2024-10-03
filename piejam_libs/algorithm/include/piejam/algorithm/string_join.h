// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ranges>
#include <string>

namespace piejam::algorithm
{

template <std::ranges::input_range Range>
auto
string_join(Range const& rng, char sep) -> std::string
{
    if (!std::ranges::empty(rng))
    {
        std::string acc(*std::ranges::begin(rng));
        for (auto const& s : std::views::drop(rng, 1))
        {
            (acc += sep) += s;
        }
        return acc;
    }

    return {};
}

} // namespace piejam::algorithm
