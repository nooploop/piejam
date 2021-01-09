// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <type_traits>

namespace piejam
{

template <class... Types>
constexpr bool is_nothrow_default_constructible_v =
        (std::is_nothrow_default_constructible_v<Types> && ...);

} // namespace piejam
