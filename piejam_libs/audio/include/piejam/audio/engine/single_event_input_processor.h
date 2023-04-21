// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/process_context.h>

namespace piejam::audio::engine
{

template <class Processor, class T>
class single_event_input_processor
{
public:
    void process_sliced(process_context const& ctx);
};

template <class Processor, class T>
void
single_event_input_processor<Processor, T>::process_sliced(
        process_context const& ctx)
{
    event_buffer<T> const& ev_in_buf = ctx.event_inputs.get<T>(0);

    auto& this_proc = static_cast<Processor&>(*this);

    if (ev_in_buf.empty())
    {
        this_proc.process_buffer(ctx);
    }
    else
    {
        std::size_t offset{};
        for (event<T> const& ev : ev_in_buf)
        {
            BOOST_ASSERT(ev.offset() < ctx.buffer_size);
            BOOST_ASSERT(offset <= ev.offset());

            if (offset != ev.offset())
            {
                this_proc.process_slice(ctx, offset, ev.offset() - offset);
            }

            this_proc.process_event(ctx, ev);
            offset = ev.offset();
        }

        this_proc.process_slice(ctx, offset, ctx.buffer_size - offset);
    }
}

} // namespace piejam::audio::engine
