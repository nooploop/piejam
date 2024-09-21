// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam
{

constexpr inline auto
toggle_bool(bool& x) -> bool
{
    return x = !x;
}

} // namespace piejam
