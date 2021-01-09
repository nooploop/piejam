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
    std::filesystem::path path;
    std::string name{"None"};
};

inline bool
operator==(pcm_descriptor const& l, pcm_descriptor const& r) noexcept
{
    return l.path == r.path && l.name == r.name;
}

inline bool
operator!=(pcm_descriptor const& l, pcm_descriptor const& r) noexcept
{
    return !(l == r);
}

struct pcm_io_descriptors
{
    std::vector<pcm_descriptor> inputs{1};
    std::vector<pcm_descriptor> outputs{1};
};

inline bool
operator==(pcm_io_descriptors const& l, pcm_io_descriptors const& r) noexcept
{
    return l.inputs == r.inputs && l.outputs == r.outputs;
}

inline bool
operator!=(pcm_io_descriptors const& l, pcm_io_descriptors const& r) noexcept
{
    return !(l == r);
}

} // namespace piejam::audio
