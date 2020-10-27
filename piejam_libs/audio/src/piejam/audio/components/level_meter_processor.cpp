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

#include <piejam/audio/components/level_meter_processor.h>

#include <piejam/audio/engine/process_context.h>

#include <algorithm>
#include <span>

namespace piejam::audio::components
{

static constexpr auto
level_meter_decay_time(unsigned const samplerate) -> std::size_t
{
    return samplerate / 10;
}

level_meter_processor::level_meter_processor(unsigned const samplerate)
    : m_lm(level_meter_decay_time(samplerate))
{
}

void
level_meter_processor::process(engine::process_context const& ctx)
{
    std::span<float const> const& in_buf = ctx.inputs[0];
    if (!in_buf.empty())
        std::ranges::copy(in_buf, std::back_inserter(m_lm));
    else
        std::fill_n(std::back_inserter(m_lm), ctx.buffer_size, 0.f);

    m_peak_level.store(m_lm.get(), std::memory_order_release);
}

} // namespace piejam::audio::components
