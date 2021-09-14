// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/component.h>

#include <gmock/gmock.h>

namespace piejam::audio::engine::test
{

struct component_mock : public component
{
    MOCK_METHOD(endpoints, inputs, (), (const, override));
    MOCK_METHOD(endpoints, outputs, (), (const, override));

    MOCK_METHOD(endpoints, event_inputs, (), (const, override));
    MOCK_METHOD(endpoints, event_outputs, (), (const, override));

    MOCK_METHOD(void, connect, (graph&), (const, override));
};

} // namespace piejam::audio::engine::test
