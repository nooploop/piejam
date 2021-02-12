// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace piejam::numeric
{

template <std::floating_point>
class rolling_mean;

} // namespace piejam::numeric
