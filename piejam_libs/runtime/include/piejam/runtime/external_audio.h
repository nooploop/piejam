// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/entity_map.h>
#include <piejam/npos.h>
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/external_audio_fwd.h>
#include <piejam/runtime/string_id.h>

namespace piejam::runtime::external_audio
{

struct device
{
    string_id name;

    audio::bus_type bus_type{};

    //! mono channels hold same device channel in the pair
    channel_index_pair channels{npos};

    auto operator==(device const&) const noexcept -> bool = default;
};

struct state
{
    devices_t devices;

    box<device_ids_t> inputs;
    box<device_ids_t> outputs;
};

} // namespace piejam::runtime::external_audio
