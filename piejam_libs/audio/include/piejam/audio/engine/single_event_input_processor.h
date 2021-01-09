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
    void process_sliced_on_events(process_context const& ctx);
};

template <class Processor, class T>
void
single_event_input_processor<Processor, T>::process_sliced_on_events(
        process_context const& ctx)
{
    event_buffer<T> const& ev_in_buf = ctx.event_inputs.get<T>(0);

    Processor& this_proc = static_cast<Processor&>(*this);

    if (ev_in_buf.empty())
    {
        this_proc.process_without_events(ctx);
    }
    else if (ev_in_buf.size() == 1 && ev_in_buf.begin()->offset() == 0)
    {
        this_proc.process_with_starting_event(ctx, ev_in_buf.begin()->value());
    }
    else
    {
        std::size_t offset{};
        for (event<T> const& ev : ev_in_buf)
        {
            BOOST_ASSERT(ev.offset() < ctx.buffer_size);
            this_proc.process_event_slice(ctx, offset, ev.offset(), ev.value());
            offset = ev.offset();
        }

        this_proc.process_final_slice(ctx, offset);
    }
}

} // namespace piejam::audio::engine
