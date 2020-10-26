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

#include <piejam/audio/engine/process_context.h>

#include <algorithm>
#include <cassert>

namespace piejam::audio::engine
{

output_processor::output_processor(std::size_t const num_inputs)
    : m_num_inputs(num_inputs)
{
    assert(m_num_inputs > 0);
}

void
output_processor::process(process_context const& ctx)
{
    assert(m_engine_output.major_size() == m_num_inputs);

    for (std::size_t ch = 0; ch < m_num_inputs; ++ch)
    {
        auto const out = m_engine_output[ch];
        if (ctx.inputs[ch].get().empty())
        {
            std::ranges::fill(out, 0.f);
        }
        else
        {
            assert(ctx.inputs[ch].get().size() == out.size());
            std::ranges::copy(ctx.inputs[ch].get(), out.begin());
        }
    }
}

} // namespace piejam::audio::engine
