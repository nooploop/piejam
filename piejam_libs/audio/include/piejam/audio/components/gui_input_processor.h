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

#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>

#include <array>
#include <atomic>
#include <functional>

namespace piejam::audio::components
{

template <class T>
class gui_input_processor final : public engine::named_processor
{
    static_assert(std::atomic<T>::is_always_lock_free);

public:
    gui_input_processor() = default;
    gui_input_processor(
            T const initial,
            std::string_view const& name = {}) noexcept
        : named_processor(name)
        , m_event_output_ports{engine::event_port{typeid(T), std::string(name)}}
        , m_value{initial}
    {
    }

    void set(T x) noexcept
    {
        m_value.store(std::move(x), std::memory_order_release);
    }

    auto type_name() const -> std::string_view override { return "gui_input"; }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }
    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override
    {
        return m_event_output_ports;
    }
    void create_event_output_buffers(
            engine::event_output_buffer_factory const& f) const override
    {
        f.add<T>();
    }

    void process(engine::process_context const& ctx) override
    {
        engine::verify_process_context(*this, ctx);
        std::invoke(m_process_fn, this, ctx);
    }

private:
    void initial_process(engine::process_context const& ctx)
    {
        T const value = m_value.load(std::memory_order_consume);
        ctx.event_outputs.get<T>(0).insert(0, value);
        m_last_sent_value = value;
        m_process_fn = &gui_input_processor<T>::subsequent_process;
    }

    void subsequent_process(engine::process_context const& ctx)
    {
        T const value = m_value.load(std::memory_order_consume);
        if (m_last_sent_value == value)
            return;

        ctx.event_outputs.get<T>(0).insert(0, value);
        m_last_sent_value = value;
    }

    std::array<engine::event_port, 1> const m_event_output_ports;

    std::atomic<T> m_value{};
    T m_last_sent_value{};

    using process_fn_t = decltype(&gui_input_processor<T>::process);
    process_fn_t m_process_fn{&gui_input_processor<T>::initial_process};
};

} // namespace piejam::audio::components
