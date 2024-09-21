// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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
    auto vec_ = vec.lock();
    boost::remove_erase(*vec_, value);
}

template <class T, class... Args>
void
emplace_back(boxed_vector<T>& vec, Args&&... args)
{
    auto vec_ = vec.lock();

    vec_->template emplace_back(std::forward<decltype(args)>(args)...);
}

} // namespace piejam
