// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/processors/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::runtime::components
{

auto make_stereo_stream(
        audio_stream_id,
        processors::stream_processor_factory&,
        std::size_t buffer_capacity_per_channel,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
