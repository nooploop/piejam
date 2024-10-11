// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/audio_stream.h>

#include <piejam/box.h>
#include <piejam/entity_data_map.h>
#include <piejam/entity_id.h>

namespace piejam::runtime
{

auto
make_stream(audio_streams_t& streams, std::size_t num_channels)
        -> audio_stream_id
{
    auto id = audio_stream_id::generate();
    streams.emplace(id, std::in_place, num_channels);
    return id;
}

} // namespace piejam::runtime
