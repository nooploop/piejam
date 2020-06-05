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

#include <piejam/audio/cpu_load_meter.h>

namespace piejam::audio
{

cpu_load_meter::cpu_load_meter(
        std::size_t const num_frames,
        unsigned const samplerate)
    : m_max_processing_time(num_frames * 1.e9f / samplerate)
    , m_processing_start(clock_t::now())
{
}

auto
cpu_load_meter::stop() -> float
{
    auto const processing_stop = clock_t::now();
    return (processing_stop - m_processing_start) / m_max_processing_time;
}

} // namespace piejam::audio
