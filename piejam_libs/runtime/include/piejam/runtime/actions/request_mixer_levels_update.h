// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <vector>

namespace piejam::runtime::actions
{

auto request_mixer_levels_update(mixer::channel_ids_t) -> thunk_action;

} // namespace piejam::runtime::actions
