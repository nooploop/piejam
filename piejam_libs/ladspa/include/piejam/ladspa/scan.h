// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/ladspa/fwd.h>

#include <filesystem>
#include <vector>

namespace piejam::ladspa
{

auto scan_file(std::filesystem::path const&) -> std::vector<plugin_descriptor>;
auto scan_directory(std::filesystem::path const&)
        -> std::vector<plugin_descriptor>;

} // namespace piejam::ladspa
