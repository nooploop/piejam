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

#include <piejam/audio/engine/processor.h>
#include <piejam/audio/level_peak_meter.h>

#include <atomic>

namespace piejam::audio::components
{

class level_meter_processor final : public engine::processor
{
public:
    level_meter_processor(unsigned samplerate);

    auto peak_level() const noexcept -> float
    {
        return m_peak_level.load(std::memory_order_consume);
    }

    auto num_inputs() const -> std::size_t override { return 1; }
    auto num_outputs() const -> std::size_t override { return 0; }
    auto num_event_inputs() const -> std::size_t override { return 0; }
    auto num_event_outputs() const -> std::size_t override { return 0; }
    void create_event_output_buffers(
            engine::event_output_buffer_factory const&) const override
    {
    }

    void process(engine::process_context const&) override;

private:
    level_peak_meter m_lm;
    std::atomic<float> m_peak_level{};
};

} // namespace piejam::audio::components
