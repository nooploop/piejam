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

#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>

#include <gmock/gmock.h>

namespace piejam::audio::engine::test
{

struct processor_mock : public processor
{
    MOCK_METHOD(std::string_view, type_name, (), (const, override));
    MOCK_METHOD(std::string_view, name, (), (const, override));

    MOCK_METHOD(std::size_t, num_inputs, (), (const, override));
    MOCK_METHOD(std::size_t, num_outputs, (), (const, override));

    MOCK_METHOD(event_ports, event_inputs, (), (const, override));
    MOCK_METHOD(event_ports, event_outputs, (), (const, override));

    MOCK_METHOD(void, process, (process_context const&), (override));
};

} // namespace piejam::audio::engine::test
