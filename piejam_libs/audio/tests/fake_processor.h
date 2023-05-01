// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/processor.h>

#include <gmock/gmock.h>

namespace piejam::audio::engine::test
{

class fake_processor : public processor
{
public:
    fake_processor(
            std::string_view name,
            std::size_t num_inputs,
            std::size_t num_outputs,
            std::vector<event_port> event_inputs = {},
            std::vector<event_port> event_outputs = {})
        : m_name{name}
        , m_num_inputs{num_inputs}
        , m_num_outputs{num_outputs}
        , m_event_inputs{std::move(event_inputs)}
        , m_event_outputs{std::move(event_outputs)}
    {
    }

    [[nodiscard]] auto type_name() const noexcept -> std::string_view override
    {
        return "fake";
    }

    [[nodiscard]] auto name() const noexcept -> std::string_view override
    {
        return m_name;
    }

    [[nodiscard]] auto num_inputs() const noexcept -> std::size_t override
    {
        return m_num_inputs;
    }

    [[nodiscard]] auto num_outputs() const noexcept -> std::size_t override
    {
        return m_num_outputs;
    }

    [[nodiscard]] auto event_inputs() const noexcept -> event_ports override
    {
        return m_event_inputs;
    }

    [[nodiscard]] auto event_outputs() const noexcept -> event_ports override
    {
        return m_event_outputs;
    }

    void process(process_context const&) override
    {
    }

private:
    std::string m_name;
    std::size_t m_num_inputs{};
    std::size_t m_num_outputs{};
    std::vector<event_port> m_event_inputs;
    std::vector<event_port> m_event_outputs;
};

} // namespace piejam::audio::engine::test
