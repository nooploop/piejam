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
    level_meter_processor(sample_rate, std::string_view name = {});

    [[nodiscard]] auto type_name() const noexcept -> std::string_view override
    {
        return "level_meter";
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
        static std::array s_ports{
                event_port(std::in_place_type<float>, "peak_level")};
        return s_ports;
    }

    void process(engine::process_context const&) override;

private:
    level_peak_meter m_lm;
};

} // namespace piejam::audio::engine
