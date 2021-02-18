// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/boxed_string.h>
#include <piejam/fwd.h>
#include <piejam/npos.h>
#include <piejam/runtime/channel_index_pair.h>

namespace piejam::runtime::device_io
{

struct bus
{
    boxed_string name;

    audio::bus_type bus_type{};

    //! mono channels hold same device channel in the pair
    channel_index_pair channels{npos};

    bool operator==(bus const&) const noexcept = default;
};

using bus_id = entity_id<bus>;

} // namespace piejam::runtime::device_io
