// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/pcm_buffer_converter.h>

#include <span>
#include <vector>

namespace piejam::audio::engine
{

class output_processor final : public named_processor
{
public:
    output_processor(std::string_view name = {});

    void set_output(pcm_output_buffer_converter const& engine_output)
    {
        m_engine_output = engine_output;
    }

    [[nodiscard]] auto type_name() const noexcept -> std::string_view override
    {
        return "output";
    }

    [[nodiscard]] auto num_inputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    [[nodiscard]] auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
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
    pcm_output_buffer_converter m_engine_output;
};

} // namespace piejam::audio::engine
