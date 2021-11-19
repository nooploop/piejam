// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam
{

struct default_t
{
    [[nodiscard]] constexpr bool
    operator==(default_t const&) const noexcept = default;
};

} // namespace piejam
