// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/output_processor.h>

#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/period_size.h>

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

    m_engine_output(ctx.inputs[0].get().as_variant());
}

} // namespace piejam::audio::engine
