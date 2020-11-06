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
    event_buffer<T> const* const ev_in_buf = ctx.event_inputs.get<T>(0);

    Processor& this_proc = static_cast<Processor&>(*this);

    if (!ev_in_buf || ev_in_buf->empty())
    {
        this_proc.process_without_events(ctx);
    }
    else if (ev_in_buf->size() == 1 && ev_in_buf->begin()->offset() == 0)
    {
        this_proc.process_with_starting_event(ctx, ev_in_buf->begin()->value());
    }
    else
    {
        std::size_t offset{};
        for (event<T> const& ev : *ev_in_buf)
        {
            BOOST_ASSERT(ev.offset() < ctx.buffer_size);
            this_proc.process_event_slice(ctx, offset, ev.offset(), ev.value());
            offset = ev.offset();
        }

        this_proc.process_final_slice(ctx, offset);
    }
}

} // namespace piejam::audio::engine
