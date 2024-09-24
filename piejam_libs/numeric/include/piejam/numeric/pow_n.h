// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <utility>

namespace piejam::numeric
{

template <std::size_t N>
constexpr auto pow_n = []<class T>(T const t) {
    return []<std::size_t... I>(T const t, std::index_sequence<I...>) {
        return (((void)I, t) * ...);
    }(t, std::make_index_sequence<N>{});
};

} // namespace piejam::numeric
