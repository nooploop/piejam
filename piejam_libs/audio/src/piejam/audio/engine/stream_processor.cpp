// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/stream_processor.h>

#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/period_sizes.h>
#include <piejam/functional/overload.h>
#include <piejam/range/iota.h>
#include <piejam/range/stride_iterator.h>

namespace piejam::audio::engine
{

stream_processor::stream_processor(
        std::size_t const num_channels,
        std::size_t const capacity_per_channel,
        std::string_view const& name)
    : named_processor(name)
    , m_num_channels(num_channels)
    , m_buffer(num_channels * capacity_per_channel)
    , m_interleave_buffer(num_channels * max_period_size)
{
}

void
stream_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    for (std::size_t const ch : range::iota(m_num_channels))
    {
        auto const& in = ctx.inputs[ch];

        std::visit(
                overload{
                        [this, ch, &ctx](float const constant) {
                            std::ranges::fill_n(
                                    range::stride_iterator(
                                            m_interleave_buffer.data() + ch,
                                            m_num_channels),
                                    ctx.buffer_size,
                                    constant);
                        },
                        [this, ch](std::span<float const> const& buffer) {
                            std::ranges::copy(
                                    buffer,
                                    range::stride_iterator(
                                            m_interleave_buffer.data() + ch,
                                            m_num_channels));
                        }},
                in.get().as_variant());

        ctx.results[ch] = in;
    }

    m_buffer.write(
            {m_interleave_buffer.data(), ctx.buffer_size * m_num_channels});
}

} // namespace piejam::audio::engine
