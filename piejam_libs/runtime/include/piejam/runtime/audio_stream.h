// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/box.h>
#include <piejam/fwd.h>

namespace piejam::runtime
{

using audio_stream_buffer = unique_box<audio::multichannel_buffer<
        float,
        audio::multichannel_layout_non_interleaved>>;
using audio_streams_cache = entity_map<audio_stream_buffer>;
using audio_stream_id = entity_id<audio_stream_buffer>;

} // namespace piejam::runtime
