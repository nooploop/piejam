// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>

#include <boost/range/algorithm_ext/erase.hpp>

#include <vector>

namespace piejam
{

template <class T>
using boxed_vector = box<std::vector<T>>;

template <class T>
auto
remove_erase(boxed_vector<T>& vec, T const& value)
{
    vec.update(
            [&value](std::vector<T>& vec) { boost::remove_erase(vec, value); });
}

} // namespace piejam
