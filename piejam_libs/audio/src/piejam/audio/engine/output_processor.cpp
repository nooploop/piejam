// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/output_processor.h>

#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>

#include <xsimd/math/xsimd_basic_math.hpp>

#include <boost/assert.hpp>

#include <algorithm>

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

    transform(ctx.inputs[0].get(), m_engine_output, [](auto&& x) {
        using x_t = std::decay_t<decltype(x)>;
        return xsimd::clip(std::forward<decltype(x)>(x), x_t{-1.f}, x_t{1.f});
    });
}

} // namespace piejam::audio::engine
