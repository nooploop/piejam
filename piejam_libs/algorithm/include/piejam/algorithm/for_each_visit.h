// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ranges>
#include <variant>

namespace piejam::algorithm
{

template <std::ranges::input_range Range, class Visitor>
constexpr void
for_each_visit(Range&& rng, Visitor&& v)
{
    for (auto&& var : std::forward<Range>(rng))
        std::visit(std::forward<Visitor>(v), std::forward<decltype(var)>(var));
}

} // namespace piejam::algorithm
