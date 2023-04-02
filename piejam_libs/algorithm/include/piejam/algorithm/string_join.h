// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>
#include <string>

namespace piejam::algorithm
{

template <class Range>
auto
string_join(Range const& rng, char sep) -> std::string
{
    using std::begin;
    using std::empty;
    using std::end;

    if (!empty(rng))
    {
        auto first = begin(rng);
        std::string acc(*first);
        std::for_each(std::next(first), end(rng), [&acc, sep](auto const& s) {
            (acc += sep) += s;
        });
        return acc;
    }
    else
        return {};
}

} // namespace piejam::algorithm
