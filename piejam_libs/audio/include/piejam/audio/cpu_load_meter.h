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

#include <chrono>

namespace piejam::audio
{

class cpu_load_meter
{
public:
    cpu_load_meter(std::size_t num_frames, unsigned samplerate);

    auto stop() -> float;

private:
    using clock_t = std::chrono::steady_clock;
    std::chrono::duration<float, clock_t::period> const m_max_processing_time;
    clock_t::time_point const m_processing_start;
};

} // namespace piejam::audio
