// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>
#include <piejam/default.h>
#include <piejam/fwd.h>
#include <piejam/runtime/external_audio_fwd.h>

#include <variant>
#include <vector>

namespace piejam::runtime::mixer
{

struct channel;
using channel_id = entity_id<channel>;
using channels_t = entity_map<channel>;

using missing_device_address = boxed_string;

using io_address_t = std::variant<
        default_t,
        missing_device_address,
        external_audio::device_id,
        channel_id>;

enum class io_socket
{
    in,
    out,
};

using channel_ids_t = std::vector<channel_id>;

struct state;

} // namespace piejam::runtime::mixer
