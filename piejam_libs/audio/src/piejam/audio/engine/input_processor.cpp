// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/input_processor.h>

#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>

#include <boost/assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

input_processor::input_processor(
        std::size_t const num_outputs,
        std::string_view const& name)
    : named_processor(name)
    , m_num_outputs(num_outputs)
{
}

void
input_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    BOOST_ASSERT(m_engine_input.minor_step() == 1);
    for (std::size_t ch = 0; ch < m_num_outputs; ++ch)
    {
        BOOST_ASSERT(m_engine_input.minor_size() == ctx.outputs[ch].size());
        ctx.results[ch] = std::span{
                m_engine_input[ch].data(),
                m_engine_input.minor_size()};
    }
}

} // namespace piejam::audio::engine
