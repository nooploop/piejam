// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::runtime::actions
{

auto reset_fx_parameter_to_default_value(fx::parameter_id) -> thunk_action;

} // namespace piejam::runtime::actions
