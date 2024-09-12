// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>
#include <piejam/io_pair.h>

#include <filesystem>
#include <string>
#include <vector>

namespace piejam::audio
{

struct pcm_descriptor
{
    std::string name;
    std::filesystem::path path;

    auto operator==(pcm_descriptor const& other) const noexcept
            -> bool = default;
};

using pcm_io_descriptors = io_pair<unique_box<std::vector<pcm_descriptor>>>;

} // namespace piejam::audio
