// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/stream_processor.h>

#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/period_size.h>
#include <piejam/range/iota.h>
#include <piejam/range/stride_pointer_iterator.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::audio::engine
{

stream_processor::stream_processor(
        std::size_t const num_channels,
        std::size_t const capacity_per_channel,
        std::string_view const name)
    : named_processor(name)
    , m_num_channels(num_channels)
    , m_buffer(num_channels, capacity_per_channel)
{
    BOOST_ASSERT(m_num_channels > 0);
}

void
stream_processor::process(process_context const& ctx)
{
    verify_process_context(*this, ctx);

    m_buffer.write(ctx.inputs, ctx.buffer_size);
}

auto
make_stream_processor(
        std::size_t const num_channels,
        std::size_t const capacity_per_channel,
        std::string_view const name) -> std::unique_ptr<stream_processor>
{
    return std::make_unique<stream_processor>(
            num_channels,
            capacity_per_channel,
            name);
}

} // namespace piejam::audio::engine
