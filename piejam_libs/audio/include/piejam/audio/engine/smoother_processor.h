// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

inline constexpr std::size_t default_smooth_length = 256;
inline constexpr std::size_t default_smooth_steps = 16;

auto make_event_to_audio_smoother_processor(
        std::size_t smooth_length = default_smooth_length,
        std::string_view name = {}) -> std::unique_ptr<processor>;

auto make_event_smoother_processor(
        std::size_t smooth_length = default_smooth_length,
        std::size_t smooth_steps = default_smooth_steps,
        std::string_view name = {}) -> std::unique_ptr<processor>;

} // namespace piejam::audio::engine
