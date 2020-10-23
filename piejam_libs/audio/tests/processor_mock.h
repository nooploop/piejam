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

#include <gmock/gmock.h>

namespace piejam::audio::engine::test
{

struct processor_mock : public processor
{
    MOCK_METHOD(std::size_t, num_inputs, (), (const, override));
    MOCK_METHOD(std::size_t, num_outputs, (), (const, override));

    MOCK_METHOD(std::size_t, num_event_inputs, (), (const, override));
    MOCK_METHOD(std::size_t, num_event_outputs, (), (const, override));

    MOCK_METHOD(
            void,
            create_event_output_buffers,
            (event_output_buffer_factory const&),
            (const, override));

    MOCK_METHOD(
            void,
            process,
            (input_buffers_t const&,
             output_buffers_t const&,
             result_buffers_t const&,
             event_input_buffers const&,
             event_output_buffers const&),
            (override));
};

} // namespace piejam::audio::engine::test
