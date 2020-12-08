// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>

#include <boost/assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

inline void
verify_process_context(processor const& proc, process_context const& ctx)
{
    BOOST_ASSERT(proc.num_inputs() == ctx.inputs.size());
    BOOST_ASSERT(proc.num_outputs() == ctx.outputs.size());
    BOOST_ASSERT(proc.num_outputs() == ctx.results.size());
    BOOST_ASSERT(proc.event_inputs().size() == ctx.event_inputs.size());
    BOOST_ASSERT(proc.event_outputs().size() == ctx.event_outputs.size());
    BOOST_ASSERT(std::ranges::all_of(ctx.inputs, [&](audio_slice const& b) {
        return b.is_constant() || b.buffer().size() == ctx.buffer_size;
    }));
    BOOST_ASSERT(
            std::ranges::all_of(ctx.outputs, [&](std::span<float> const& b) {
                return b.data() && b.size() == ctx.buffer_size;
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

    boost::ignore_unused(proc, ctx);
}

} // namespace piejam::audio::engine
