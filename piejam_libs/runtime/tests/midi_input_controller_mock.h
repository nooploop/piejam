// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/midi_input_controller.h>

#include <piejam/entity_id.h>
#include <piejam/midi/input_event_handler.h>

#include <gmock/gmock.h>

namespace piejam::runtime::test
{

struct midi_input_controller_mock : public midi_input_controller
{
    MOCK_METHOD(bool, activate_input_device, (midi::device_id_t));
    MOCK_METHOD(void, deactivate_input_device, (midi::device_id_t));
    MOCK_METHOD(
            std::unique_ptr<midi::input_event_handler>,
            make_input_event_handler,
            ());
};

} // namespace piejam::runtime::test
