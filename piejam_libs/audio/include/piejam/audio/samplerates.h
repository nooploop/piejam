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

#include <piejam/audio/types.h>

#include <boost/container/static_vector.hpp>

#include <array>

namespace piejam::audio
{

inline constexpr std::array preferred_samplerates{
        11025u,
        12000u,
        22050u,
        24000u,
        44100u,
        48000u,
        88200u,
        96000u,
        176400u,
        192000u};

using samplerates_t = boost::container::
        static_vector<samplerate_t, preferred_samplerates.size()>;

} // namespace piejam::audio
