// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/pcm_buffer_converter.h>

namespace piejam::audio::engine
{

class input_processor final : public named_processor
{
public:
    input_processor(std::string_view name = {});

    void set_input(pcm_input_buffer_converter const& engine_input)
    {
        m_engine_input = engine_input;
    }

    [[nodiscard]] auto type_name() const noexcept -> std::string_view override
    {
        return "input";
    }

    [[nodiscard]] auto num_inputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    [[nodiscard]] auto num_outputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    [[nodiscard]] auto event_inputs() const noexcept -> event_ports override
    {
        return {};
    }

    [[nodiscard]] auto event_outputs() const noexcept -> event_ports override
    {
        return {};
    }

    void process(process_context const&) override;

private:
    pcm_input_buffer_converter m_engine_input;
};

} // namespace piejam::audio::engine
