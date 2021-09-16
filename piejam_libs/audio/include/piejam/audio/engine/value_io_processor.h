// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/thread/spsc_slot.h>

#include <array>

namespace piejam::audio::engine
{

template <class T>
class value_io_processor final : public engine::named_processor
{
public:
    value_io_processor(std::string_view const& name = {})
        : named_processor(name)
        , m_type_name("value_io")
    {
    }

    void set(T const x) noexcept { m_in_value.push(x); }

    bool get(T& x) noexcept { return m_out_value.pull(x); }

    template <class F>
    void consume(F&& f)
    {
        m_out_value.consume(std::forward<F>(f));
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return m_type_name;
    }

    auto num_inputs() const noexcept -> std::size_t override { return 0; }
    auto num_outputs() const noexcept -> std::size_t override { return 0; }

    auto event_inputs() const noexcept -> event_ports override
    {
        static std::array s_ports{event_port(std::in_place_type<T>, "in")};
        return s_ports;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        static std::array s_ports{event_port(std::in_place_type<T>, "out")};
        return s_ports;
    }

    void process(engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        auto& out = ctx.event_outputs.get<T>(0);

        m_in_value.consume([&out](T const& value) { out.insert(0, value); });

        for (event<T> const& ev : ctx.event_inputs.get<T>(0))
        {
            m_out_value.push(ev.value());
            out.insert(ev.offset(), ev.value());
        }
    }

private:
    std::string m_type_name;

    thread::spsc_slot<T> m_in_value;
    thread::spsc_slot<T> m_out_value;
};

} // namespace piejam::audio::engine
