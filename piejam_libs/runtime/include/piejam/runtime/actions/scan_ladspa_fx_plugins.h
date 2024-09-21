// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <filesystem>

namespace piejam::runtime::actions
{

auto scan_ladspa_fx_plugins(std::filesystem::path const& dir) -> thunk_action;

} // namespace piejam::runtime::actions
