// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
    MOCK_METHOD(std::string_view, type_name, (), (const, noexcept, override));
    MOCK_METHOD(std::string_view, name, (), (const, noexcept, override));

    MOCK_METHOD(std::size_t, num_inputs, (), (const, noexcept, override));
    MOCK_METHOD(std::size_t, num_outputs, (), (const, noexcept, override));

    MOCK_METHOD(event_ports, event_inputs, (), (const, noexcept, override));
    MOCK_METHOD(event_ports, event_outputs, (), (const, noexcept, override));

    MOCK_METHOD(void, process, (process_context const&), (override));
};

} // namespace piejam::audio::engine::test
