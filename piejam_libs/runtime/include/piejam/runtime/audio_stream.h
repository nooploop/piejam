// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_vector.h>
#include <piejam/fwd.h>

namespace piejam::runtime
{

using audio_stream_buffer = boxed_vector<float>;
using audio_streams_cache = entity_map<audio_stream_buffer>;
using audio_stream_id = entity_id<audio_stream_buffer>;

} // namespace piejam::runtime
