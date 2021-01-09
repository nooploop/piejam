// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameters.h>

namespace piejam::runtime::actions
{

auto set_float_parameter_normalized(float_parameter_id, float norm_value) -> thunk_action;

} // namespace piejam::runtime::actions
