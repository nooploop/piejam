// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/mixer_fwd.h>

#include <boost/container/container_fwd.hpp>

namespace piejam::runtime
{

using recorder_streams_t =
        boost::container::flat_map<mixer::channel_id, audio_stream_id>;

} // namespace piejam::runtime
