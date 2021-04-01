// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/stream_capture_processor.h>

#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/functional/overload.h>

#include <boost/hof/always.hpp>
#include <boost/lockfree/spsc_queue.hpp>

namespace piejam::audio::engine
{

stream_capture_processor::stream_capture_processor(
        std::size_t capacity,
        std::string_view const& name)
    : named_processor(name)
    , m_buffer(capacity)
{
}

void
stream_capture_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    auto const& in = ctx.inputs[0];

    std::visit(
            overload{
                    [this, &ctx](float const constant) {
                        // use the output buffer as temporary storage
                        std::ranges::fill(ctx.outputs[0], constant);
                        m_buffer.push(
                                ctx.outputs[0].data(),
                                ctx.outputs[0].size());
                    },
                    [this](std::span<float const> const& buffer) {
                        m_buffer.push(buffer.data(), buffer.size());
                    }},
            in.get().as_variant());

    ctx.results[0] = in;
}

} // namespace piejam::audio::engine
