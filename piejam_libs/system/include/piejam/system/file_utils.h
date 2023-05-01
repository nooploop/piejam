// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <string_view>

namespace piejam::system
{

[[nodiscard]] auto make_unique_filename(
        std::filesystem::path const& base_dir,
        std::string_view base_filename,
        std::string_view extension) -> std::filesystem::path;

} // namespace piejam::system
