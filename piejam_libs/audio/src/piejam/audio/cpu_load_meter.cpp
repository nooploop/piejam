// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
