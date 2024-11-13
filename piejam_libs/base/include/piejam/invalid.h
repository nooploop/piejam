// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <compare>

namespace piejam
{

struct invalid_t
{
    [[nodiscard]]
    constexpr auto operator<=>(invalid_t const&) const noexcept = default;
};

} // namespace piejam
