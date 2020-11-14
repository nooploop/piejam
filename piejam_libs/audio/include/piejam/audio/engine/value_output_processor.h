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
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/thread/spsc_slot.h>

#include <array>

namespace piejam::audio::engine
{

template <class T>
class value_output_processor final : public engine::named_processor
{
    static_assert(std::atomic<T>::is_always_lock_free);

public:
    value_output_processor(std::string_view const& name = {})
        : named_processor(name)
        , m_event_input_ports{
                  engine::event_port{std::in_place_type<T>, std::string(name)}}
    {
    }

    bool get(T& x) { return m_value.pull(x); }

    template <class F>
    void consume(F&& f)
    {
        m_value.consume(std::forward<F>(f));
    }

    auto type_name() const -> std::string_view override
    {
        return "value_output";
    }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override
    {
        return m_event_input_ports;
    }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(engine::process_context const& ctx) override
    {
        engine::verify_process_context(*this, ctx);

        event_buffer<T> const& in_buf = ctx.event_inputs.get<T>(0);

        for (event<T> const& ev : in_buf)
            m_value.push(ev.value());
    }

private:
    std::array<engine::event_port, 1> const m_event_input_ports;

    thread::spsc_slot<T> m_value;
};

} // namespace piejam::audio::engine
