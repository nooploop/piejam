// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/midi/fwd.h>
#include <piejam/runtime/fwd.h>

#include <string>
#include <vector>

namespace piejam::runtime
{

class midi_control_middleware final
{
public:
    using device_updates_f = std::function<std::vector<midi::device_update>()>;

    midi_control_middleware(device_updates_f);

    void operator()(middleware_functors const&, action const&);

private:
    void process_device_update(
            middleware_functors const&,
            midi::device_added const&);
    void process_device_update(
            middleware_functors const&,
            midi::device_removed const&);

    template <class Action>
    void process_midi_control_action(middleware_functors const&, Action const&);

    device_updates_f m_device_updates;
    std::vector<std::string> m_enabled_devices;
};

} // namespace piejam::runtime
