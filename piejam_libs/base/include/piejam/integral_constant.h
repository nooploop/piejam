// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <type_traits>

namespace piejam
{

template <auto V>
using make_integral_constant = std::integral_constant<decltype(V), V>;

} // namespace piejam
