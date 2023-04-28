// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::components
{

auto make_amplifier(std::size_t num_channels, std::string_view name)
        -> std::unique_ptr<engine::component>;

auto make_mono_amplifier(std::string_view name)
        -> std::unique_ptr<engine::component>;

//! Applies gain to both input channels.
auto make_stereo_amplifier(std::string_view name)
        -> std::unique_ptr<engine::component>;

auto make_split_amplifier(std::size_t num_channels, std::string_view name)
        -> std::unique_ptr<engine::component>;

//! Applies separate gains to both input channels.
auto make_stereo_split_amplifier(std::string_view name)
        -> std::unique_ptr<engine::component>;

} // namespace piejam::audio::components
