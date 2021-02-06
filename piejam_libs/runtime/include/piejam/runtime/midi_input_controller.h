// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/midi/device_id.h>
#include <piejam/midi/fwd.h>

#include <memory>

namespace piejam::runtime
{

class midi_input_controller
{
public:
    virtual ~midi_input_controller() = default;

    virtual auto activate_input_device(midi::device_id_t) -> bool = 0;
    virtual auto deactivate_input_device(midi::device_id_t) -> void = 0;
    virtual auto make_input_event_handler()
            -> std::unique_ptr<midi::input_event_handler> = 0;
};

auto make_midi_input_controller(midi::device_manager&)
        -> std::unique_ptr<midi_input_controller>;

auto make_dummy_midi_input_controller()
        -> std::unique_ptr<midi_input_controller>;

} // namespace piejam::runtime
