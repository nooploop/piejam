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

output_processor::output_processor(
        std::size_t const num_inputs,
        std::string_view const& name)
    : named_processor(name)
    , m_num_inputs(num_inputs)
{
}

void
output_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    BOOST_ASSERT(m_engine_output.minor_step() == 1);
    if (m_engine_output.minor_size() == ctx.buffer_size)
    {
        for (std::size_t ch = 0; ch < m_num_inputs; ++ch)
        {
            std::span out(
                    m_engine_output[ch].data(),
                    m_engine_output.minor_size());
            copy(transform(
                         ctx.inputs[ch].get(),
                         out,
                         [](auto&& x) {
                             using x_t = std::decay_t<decltype(x)>;
                             return xsimd::clip(
                                     std::forward<decltype(x)>(x),
                                     x_t{-1.f},
                                     x_t{1.f});
                         }),
                 out);
        }
    }
}

} // namespace piejam::audio::engine
