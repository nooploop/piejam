// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

#include <vector>

namespace piejam::runtime::persistence
{

using fx_midi_assignments = std::vector<fx::parameter_midi_assignment>;

} // namespace piejam::runtime::persistence
