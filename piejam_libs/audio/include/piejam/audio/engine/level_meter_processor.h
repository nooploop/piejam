// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/fwd.h>
#include <piejam/audio/level_peak_meter.h>

#include <atomic>

namespace piejam::audio::engine
{

class level_meter_processor final : public named_processor
{
public:
    level_meter_processor(
            sample_rate const&,
            std::string_view const& name = {});

    auto type_name() const -> std::string_view override
    {
        return "level_meter";
    }

    auto num_inputs() const -> std::size_t override { return 1; }
    auto num_outputs() const -> std::size_t override { return 0; }
    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override
    {
        static std::array s_ports{
                event_port(std::in_place_type<float>, "peak_level")};
        return s_ports;
    }

    void process(engine::process_context const&) override;

private:
    level_peak_meter m_lm;
};

} // namespace piejam::audio::engine
