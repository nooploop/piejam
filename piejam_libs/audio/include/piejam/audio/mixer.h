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

#include <piejam/audio/pair.h>
#include <piejam/container/boxed_string.h>

#include <string>
#include <variant>
#include <vector>

namespace piejam::audio::mixer
{

using stereo_level = pair<float>;

struct channel
{
    container::boxed_string name;
    float gain{1.f};
    stereo_level level;
};

struct mono_channel : channel
{
    std::size_t device_channel{};
    float pan{};
};

struct stereo_channel : channel
{
    float balance{};
};

using input_channels = std::vector<mono_channel>;

struct state
{
    input_channels inputs;
    stereo_channel output;
};

} // namespace piejam::audio::mixer
