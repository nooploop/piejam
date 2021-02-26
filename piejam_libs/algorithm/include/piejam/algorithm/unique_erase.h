// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>

namespace piejam::algorithm
{

template <class Container>
void
unique_erase(Container& c)
{
    c.erase(std::unique(c.begin(), c.end()), c.end());
}

template <class Container, class BinaryPredicate>
void
unique_erase(Container& c, BinaryPredicate&& p)
{
    c.erase(std::unique(c.begin(), c.end(), std::forward<BinaryPredicate>(p)),
            c.end());
}

} // namespace piejam::algorithm
