// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/named_processor.h>

#include <span>
#include <vector>

namespace piejam::audio::engine
{

class output_processor final : public named_processor
{
public:
    output_processor(std::string_view const& name = {});

    void set_output(std::span<float> const& engine_output)
    {
        m_engine_output = engine_output;
    }

    auto type_name() const -> std::string_view override { return "output"; }

    auto num_inputs() const -> std::size_t override { return 1; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(process_context const&) override;

private:
    std::span<float> m_engine_output;
};

} // namespace piejam::audio::engine
