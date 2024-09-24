// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::system
{

[[nodiscard]]
auto cpu_temp() noexcept -> int;

} // namespace piejam::system
