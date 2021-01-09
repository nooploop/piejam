// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

namespace piejam::runtime::actions
{

auto reload_missing_plugins() -> thunk_action;

} // namespace piejam::runtime::actions
