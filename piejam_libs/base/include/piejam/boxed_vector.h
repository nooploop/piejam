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

template <class T, class Eq>
auto
remove_erase(detail::box<std::vector<T>, Eq>& vec, T const& value)
{
    vec.update(
            [&value](std::vector<T>& vec) { boost::remove_erase(vec, value); });
}

template <class T, class Eq, class... Args>
void
emplace_back(detail::box<std::vector<T>, Eq>& vec, Args&&... args)
{
    vec.update([&](std::vector<T>& vec) {
        vec.emplace_back(std::forward<decltype(args)>(args)...);
    });
}

} // namespace piejam
