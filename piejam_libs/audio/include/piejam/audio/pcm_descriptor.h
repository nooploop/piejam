// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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

struct pcm_io_descriptors
{
    std::vector<pcm_descriptor> inputs;
    std::vector<pcm_descriptor> outputs;

    auto operator==(pcm_io_descriptors const&) const noexcept -> bool = default;
};

} // namespace piejam::audio
