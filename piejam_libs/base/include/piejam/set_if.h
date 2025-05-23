// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>
#include <functional>
#include <utility>

namespace piejam
{

template <class T, std::invocable<T const&> P, class V>
auto
set_if(T& t, P&& pred, V&& value)
{
    if (std::invoke(std::forward<P>(pred), t))
    {
        t = std::forward<V>(value);
    }
}

} // namespace piejam
