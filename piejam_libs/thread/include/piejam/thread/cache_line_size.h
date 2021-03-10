// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace piejam::thread
{

inline constexpr std::size_t cache_line_size = 64;

} // namespace piejam::thread
