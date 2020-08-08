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

#include <piejam/algorithm/npos.h>
#include <piejam/audio/pair.h>
#include <piejam/audio/types.h>
#include <piejam/container/boxed_string.h>

#include <string>
#include <variant>
#include <vector>

namespace piejam::audio::mixer
{

using stereo_level = pair<float>;
using channel_type = bus_type;

struct channel
{
    container::boxed_string name;
    float gain{1.f};
    float pan_balance{};
    stereo_level level;
    channel_type type{};

    //! for mono channels we just use left part of the pair to store the device
    //! channel
    channel_index_pair device_channels{algorithm::npos};
};

using channels = std::vector<channel>;

struct state
{
    channels inputs;
    channels outputs;
};

} // namespace piejam::audio::mixer
