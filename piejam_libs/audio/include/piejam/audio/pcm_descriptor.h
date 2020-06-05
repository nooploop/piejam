// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
