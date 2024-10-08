// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <piejam/redux/fwd.h>

namespace piejam::runtime
{

using store = redux::store<state, action>;

} // namespace piejam::runtime
