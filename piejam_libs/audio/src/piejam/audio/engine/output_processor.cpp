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

#include <piejam/audio/engine/output_processor.h>

#include <algorithm>
#include <cassert>

namespace piejam::audio::engine
{

output_processor::output_processor(std::size_t const num_inputs)
    : m_num_inputs(num_inputs)
{
    assert(m_num_inputs > 0);
}

auto
output_processor::num_inputs() const -> std::size_t
{
    return m_num_inputs;
}

auto
output_processor::num_outputs() const -> std::size_t
{
    return 0;
}

void
output_processor::process(
        input_buffers_t const& in_bufs,
        output_buffers_t const& /*out_bufs*/,
        result_buffers_t const& /*result_bufs*/)
{
    assert(in_bufs.size() == m_num_inputs);
    assert(m_engine_output.major_size() == m_num_inputs);

    for (std::size_t ch = 0; ch < m_num_inputs; ++ch)
    {
        auto const out = m_engine_output[ch];
        if (in_bufs[ch].get().empty())
        {
            std::ranges::fill(out, 0.f);
        }
        else
        {
            assert(in_bufs[ch].get().size() == out.size());
            std::ranges::copy(in_bufs[ch].get(), out.begin());
        }
    }
}

} // namespace piejam::audio::engine
