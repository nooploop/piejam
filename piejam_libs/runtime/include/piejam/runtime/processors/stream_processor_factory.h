// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/entity_id_hash.h>
#include <piejam/runtime/audio_stream.h>

#include <memory>
#include <unordered_map>

namespace piejam::runtime::processors
{

class stream_processor_factory
{
public:
    using processor_t = audio::engine::stream_processor;
    using processor_map =
            std::unordered_map<audio_stream_id, std::weak_ptr<processor_t>>;

    ~stream_processor_factory();

    auto make_processor(
            audio_stream_id,
            std::size_t num_channels,
            std::size_t capacity_per_channel,
            std::string_view name = {}) -> std::shared_ptr<processor_t>;

    auto find_processor(audio_stream_id) const -> std::shared_ptr<processor_t>;

    void clear_expired();

private:
    processor_map m_procs;
};

} // namespace piejam::runtime::processors
