// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>

namespace piejam::system
{

auto
disk_usage(std::filesystem::path const& p)
{
    auto info = std::filesystem::space(p);
    auto const capacity = static_cast<float>(info.capacity);
    return (capacity - static_cast<float>(info.free)) / capacity;
}

} // namespace piejam::system
