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

input_processor::input_processor(std::string_view const name)
    : named_processor(name)
{
}

void
input_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    m_engine_input(ctx.outputs[0]);
    ctx.results[0] = ctx.outputs[0];
}

} // namespace piejam::audio::engine
