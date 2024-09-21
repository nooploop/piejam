// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id_hash.h>
#include <piejam/midi/device_id.h>
#include <piejam/runtime/fwd.h>

#include <unordered_map>

namespace piejam::runtime
{

using midi_devices_t =
        std::unordered_map<midi::device_id_t, midi_device_config>;

} // namespace piejam::runtime
