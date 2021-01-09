// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/named_processor.h>
#include <piejam/range/table_view.h>

#include <vector>

namespace piejam::audio::engine
{

class input_processor final : public named_processor
{
public:
    input_processor(std::size_t num_outputs, std::string_view const& name = {});

    void set_input(range::table_view<float const> const& engine_input)
    {
        m_engine_input = engine_input;
    }

    auto type_name() const -> std::string_view override { return "input"; }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return m_num_outputs; }

    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(process_context const&) override;

private:
    std::size_t const m_num_outputs{};

    range::table_view<float const> m_engine_input;
};

} // namespace piejam::audio::engine
