// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/output_processor.h>

#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/period_sizes.h>

#include <boost/assert.hpp>

#include <array>

namespace piejam::audio::engine
{

output_processor::output_processor(std::string_view const& name)
    : named_processor(name)
{
}

void
output_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    if (ctx.inputs[0].get().is_buffer())
    {
        m_engine_output(ctx.inputs[0].get().buffer());
    }
    else
    {
        std::array<float, max_period_size> out;
        std::ranges::fill_n(
                out.begin(),
                ctx.buffer_size,
                ctx.inputs[0].get().constant());
        m_engine_output(std::span(out.data(), ctx.buffer_size));
    }
}

} // namespace piejam::audio::engine
