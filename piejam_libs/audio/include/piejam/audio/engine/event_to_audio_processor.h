// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

inline constexpr std::size_t default_event_to_audio_smooth_length = 256;

auto make_event_to_audio_processor(
        std::string_view const& name = {},
        std::size_t smooth_length = default_event_to_audio_smooth_length)
        -> std::unique_ptr<processor>;

} // namespace piejam::audio::engine
