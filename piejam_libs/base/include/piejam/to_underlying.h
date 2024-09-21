// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <type_traits>

namespace piejam
{

// from C++23
template <class Enum>
constexpr auto
to_underlying(Enum e) noexcept -> std::underlying_type_t<Enum>
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

} // namespace piejam
