// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/midi_device_id.h>
#include <piejam/boxed_string.h>
#include <piejam/entity_id.h>

namespace piejam::runtime
{

struct midi_device_config
{
    boxed_string name;
    bool enabled{};
};

} // namespace piejam::runtime
