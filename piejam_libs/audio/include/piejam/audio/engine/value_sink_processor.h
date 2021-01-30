// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>

#include <array>

namespace piejam::audio::engine
{

template <class T>
class value_sink_processor final : public engine::named_processor
{
public:
    value_sink_processor(std::string_view const& name = {})
        : named_processor(name)
        , m_event_input_ports{
                  engine::event_port{std::in_place_type<T>, std::string(name)}}
    {
    }

    auto type_name() const -> std::string_view override
    {
        return "value_sink";
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
    }

private:
    std::array<engine::event_port, 1> const m_event_input_ports;
};

} // namespace piejam::audio::engine
