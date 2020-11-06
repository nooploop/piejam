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
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/smoother.h>

#include <type_traits>

namespace piejam::audio::engine
{

template <class T>
class smoother_processor : public named_processor
{
    static_assert(std::is_convertible_v<T, float>);

public:
    static constexpr std::size_t default_smooth_length = 128;

    smoother_processor(
            std::string_view const& name = {},
            std::size_t smooth_length = default_smooth_length)
        : named_processor(name)
        , m_input_events_ports{event_port{typeid(T), std::string(name)}}
        , m_smooth_length(smooth_length)
    {
    }

    auto type_name() const -> std::string_view override { return "smoother"; }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto event_inputs() const -> event_ports override
    {
        return m_input_events_ports;
    }
    auto event_outputs() const -> event_ports override { return {}; }

    void create_event_output_buffers(
            event_output_buffer_factory const&) const override
    {
    }

    void process(engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        event_buffer<T> const* const ev_buf = ctx.event_inputs.get<T>(0);
        std::span<float> const out = ctx.outputs[0];
        audio_slice& res = ctx.results[0];

        if (ev_buf && !ev_buf->empty())
        {
            std::size_t offset{};
            auto it_out = out.begin();
            for (event<T> const& ev : *ev_buf)
            {
                BOOST_ASSERT(ev.offset() < ctx.buffer_size);
                it_out = std::copy_n(
                        m_smoother.advance_iterator(),
                        ev.offset() - offset,
                        it_out);
                m_smoother.set(ev.value(), m_smooth_length);
                offset = ev.offset();
            }

            std::copy_n(
                    m_smoother.advance_iterator(),
                    ctx.buffer_size - offset,
                    it_out);
        }
        else
        {
            if (m_smoother.is_running())
            {
                std::copy_n(
                        m_smoother.advance_iterator(),
                        ctx.buffer_size,
                        out.begin());
            }
            else
            {
                res = m_smoother.current();
            }
        }
    }

private:
    std::array<event_port, 1> const m_input_events_ports;
    std::size_t const m_smooth_length{};
    smoother<T> m_smoother;
};

} // namespace piejam::audio::engine
