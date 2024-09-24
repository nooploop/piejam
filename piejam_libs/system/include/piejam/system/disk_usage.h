// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>

namespace piejam::system
{

[[nodiscard]]
auto
disk_usage(std::filesystem::path const& p)
{
    std::error_code ec;
    if (auto info = std::filesystem::space(p, ec); !ec)
    {
        auto const capacity = static_cast<float>(info.capacity);
        return (capacity - static_cast<float>(info.free)) / capacity;
    }

    return 0.f;
}

} // namespace piejam::system
