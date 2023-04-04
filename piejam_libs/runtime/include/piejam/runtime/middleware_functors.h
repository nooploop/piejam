// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <piejam/redux/middleware_functors.h>

namespace piejam::runtime
{

using middleware_functors = redux::middleware_functors<state, action>;

} // namespace piejam::runtime
