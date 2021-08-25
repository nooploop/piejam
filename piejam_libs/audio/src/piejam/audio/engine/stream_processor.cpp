// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/stream_processor.h>

#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/period_size.h>
#include <piejam/range/iota.h>
#include <piejam/range/stride_iterator.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::audio::engine
{

stream_processor::stream_processor(
        std::size_t const num_channels,
        std::size_t const capacity_per_channel,
        std::string_view const& name)
    : named_processor(name)
    , m_num_channels(num_channels)
    , m_stream_fn(get_stream_fn(num_channels))
    , m_buffer(num_channels * capacity_per_channel)
    , m_interleave_buffer(num_channels * max_period_size.get())
{
    BOOST_ASSERT(m_num_channels > 0);
}

void
stream_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    std::invoke(m_stream_fn, this, ctx);
}

void
stream_processor::stream_1(process_context const& ctx)
{
    std::visit(
            boost::hof::match(
                    [this,
                     buffer_size = ctx.buffer_size](float const constant) {
                        std::ranges::fill_n(
                                m_interleave_buffer.begin(),
                                buffer_size,
                                constant);

                        m_buffer.write(
                                {m_interleave_buffer.data(), buffer_size});
                    },
                    [this](std::span<float const> const& buffer) {
                        m_buffer.write(buffer);
                    }),
            ctx.inputs[0].get().as_variant());
}

void
stream_processor::stream_2(process_context const& ctx)
{
    std::span out{m_interleave_buffer.data(), 2 * ctx.buffer_size};

    interleave(ctx.inputs[0].get(), ctx.inputs[1].get(), out);

    m_buffer.write(out);
}

void
stream_processor::stream_4(process_context const& ctx)
{
    std::span out{m_interleave_buffer.data(), 4 * ctx.buffer_size};

    interleave(
            ctx.inputs[0].get(),
            ctx.inputs[1].get(),
            ctx.inputs[2].get(),
            ctx.inputs[3].get(),
            out);

    m_buffer.write(out);
}

void
stream_processor::stream_n(process_context const& ctx)
{
    for (std::size_t const ch : range::iota(m_num_channels))
    {
        auto const& in = ctx.inputs[ch];

        std::visit(
                boost::hof::match(
                        [this, ch, buffer_size = ctx.buffer_size](
                                float const constant) {
                            std::ranges::fill_n(
                                    range::stride_iterator(
                                            m_interleave_buffer.data() + ch,
                                            m_num_channels),
                                    buffer_size,
                                    constant);
                        },
                        [this, ch](std::span<float const> const& buffer) {
                            std::ranges::copy(
                                    buffer,
                                    range::stride_iterator(
                                            m_interleave_buffer.data() + ch,
                                            m_num_channels));
                        }),
                in.get().as_variant());
    }

    m_buffer.write(
            {m_interleave_buffer.data(), ctx.buffer_size * m_num_channels});
}

auto
stream_processor::get_stream_fn(std::size_t const num_channels) -> stream_fn_t
{
    switch (num_channels)
    {
        case 1:
            return &stream_processor::stream_1;

        case 2:
            return &stream_processor::stream_2;

        case 4:
            return &stream_processor::stream_4;

        default:
            return &stream_processor::stream_n;
    }
}

} // namespace piejam::audio::engine
