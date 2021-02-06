// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/midi/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/middleware_functors.h>

namespace piejam::runtime
{

class midi_control_middleware final : private middleware_functors
{
public:
    midi_control_middleware(middleware_functors, midi::device_manager&);

    void operator()(action const&);

private:
    void refresh_midi_devices();

    midi::device_manager& m_midi_device_manager;
};

} // namespace piejam::runtime
