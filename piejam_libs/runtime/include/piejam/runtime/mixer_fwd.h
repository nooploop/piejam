// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/default.h>
#include <piejam/fwd.h>
#include <piejam/invalid.h>
#include <piejam/runtime/external_audio_fwd.h>

#include <variant>
#include <vector>

namespace piejam::runtime::mixer
{

struct channel;
using channel_id = entity_id<channel>;
using channels_t = entity_map<channel>;

using io_address_t = std::
        variant<default_t, invalid_t, external_audio::device_id, channel_id>;

enum class io_socket
{
    in,
    out,
    aux,
};

using channel_ids_t = std::vector<channel_id>;

struct state;

} // namespace piejam::runtime::mixer
