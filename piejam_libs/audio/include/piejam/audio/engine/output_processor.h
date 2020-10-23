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
#include <piejam/range/table_view.h>

#include <vector>

namespace piejam::audio::engine
{

class output_processor final : public processor
{
public:
    output_processor(std::size_t num_inputs);

    void set_output(range::table_view<float> const& engine_output)
    {
        m_engine_output = engine_output;
    }

    auto num_inputs() const -> std::size_t override { return m_num_inputs; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto num_event_inputs() const -> std::size_t override { return 0; }
    auto num_event_outputs() const -> std::size_t override { return 0; }

    void create_event_output_buffers(
            event_output_buffer_factory const&) const override
    {
    }

    void
    process(input_buffers_t const&,
            output_buffers_t const&,
            result_buffers_t const&,
            event_input_buffers const&,
            event_output_buffers const&) override;

private:
    std::size_t const m_num_inputs{};

    range::table_view<float> m_engine_output;
};

} // namespace piejam::audio::engine
