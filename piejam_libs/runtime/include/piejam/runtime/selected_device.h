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

#include <piejam/audio/pcm_hw_params.h>
#include <piejam/box.h>

namespace piejam::runtime
{

struct selected_device
{
    std::size_t index{};
    box<audio::pcm_hw_params> hw_params;
};

inline bool
operator==(selected_device const& l, selected_device const& r) noexcept
{
    return l.index == r.index && l.hw_params == r.hw_params;
}

} // namespace piejam::runtime
