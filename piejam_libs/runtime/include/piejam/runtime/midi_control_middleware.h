// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/midi/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/middleware_functors.h>

#include <string>
#include <vector>

namespace piejam::runtime
{

class midi_control_middleware final : private middleware_functors
{
public:
    using device_updates_f = std::function<std::vector<midi::device_update>()>;

    midi_control_middleware(middleware_functors, device_updates_f);

    void operator()(action const&);

private:
    void process_device_update(midi::device_added const&);
    void process_device_update(midi::device_removed const&);

    template <class Action>
    void process_midi_control_action(Action const&);

    device_updates_f m_device_updates;
    std::vector<std::string> m_enabled_devices;
};

} // namespace piejam::runtime
