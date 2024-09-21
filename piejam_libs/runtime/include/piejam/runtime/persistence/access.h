// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <filesystem>
#include <string>
#include <vector>

namespace piejam::runtime::persistence
{

void save_app_config(
        std::filesystem::path const&,
        std::vector<std::string> const& enabled_midi_input_devices,
        state const&);

void save_session(std::filesystem::path const&, state const&);

} // namespace piejam::runtime::persistence
