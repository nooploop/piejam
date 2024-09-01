// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/audio/simd.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

inline void
verify_process_context(
        [[maybe_unused]] processor const& proc,
        [[maybe_unused]] process_context const& ctx)
{
    BOOST_ASSERT(proc.num_inputs() == ctx.inputs.size());
    BOOST_ASSERT(proc.num_outputs() == ctx.outputs.size());
    BOOST_ASSERT(proc.num_outputs() == ctx.results.size());
    BOOST_ASSERT(proc.event_inputs().size() == ctx.event_inputs.size());
    BOOST_ASSERT(proc.event_outputs().size() == ctx.event_outputs.size());
    BOOST_ASSERT(std::ranges::all_of(ctx.inputs, [&](audio_slice const& b) {
        return b.is_constant() || (b.span().size() == ctx.buffer_size &&
                                   simd::is_aligned(b.span().data()));
    }));
    BOOST_ASSERT(
            std::ranges::all_of(ctx.outputs, [&](std::span<float> const b) {
                return b.data() && (b.size() == ctx.buffer_size &&
                                    simd::is_aligned(b.data()));
            }));
    BOOST_ASSERT(std::ranges::equal(
            proc.event_inputs(),
            ctx.event_inputs,
            [](event_port const& p, abstract_event_buffer const* b) {
                return !b || p.type() == b->type();
            }));
    BOOST_ASSERT(std::ranges::equal(
            proc.event_outputs(),
            ctx.event_outputs,
            [](event_port const& p, auto const& b) {
                return p.type() == b->type();
            }));
}

} // namespace piejam::audio::engine
