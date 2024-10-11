// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream_id.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/fwd.h>

namespace piejam::runtime
{

using audio_stream_buffer = box<audio::multichannel_buffer<
        float,
        audio::multichannel_layout_non_interleaved>>;
using audio_streams_t = entity_data_map<audio_stream_id, audio_stream_buffer>;

auto make_stream(audio_streams_t&, std::size_t num_channels) -> audio_stream_id;

} // namespace piejam::runtime
