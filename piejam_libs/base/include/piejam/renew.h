// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <utility>

namespace piejam
{

template <class T, class... Args>
auto
renew(T& t, Args&&... args)
{
    t = T{std::forward<Args>(args)...};
}

} // namespace piejam
