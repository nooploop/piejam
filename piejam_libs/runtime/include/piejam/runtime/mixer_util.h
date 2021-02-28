// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::runtime
{

auto mixer_bus_input_type(
        mixer::buses_t const&,
        mixer::bus_id,
        device_io::buses_t const&) -> audio::bus_type;

} // namespace piejam::runtime
