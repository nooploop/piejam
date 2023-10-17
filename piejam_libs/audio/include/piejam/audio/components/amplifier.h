// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::components
{

// audio in: num_channels
// audio out: num_channels
// event in: gain
auto make_amplifier(std::size_t num_channels, std::string_view name)
        -> std::unique_ptr<engine::component>;

inline auto
make_mono_amplifier(std::string_view name) -> std::unique_ptr<engine::component>
{
    return make_amplifier(1, name);
}

inline auto
make_stereo_amplifier(std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return make_amplifier(2, name);
}

// audio in: num_channels
// audio out: num_channels
// event in: gain x num_channels
auto make_split_amplifier(std::size_t num_channels, std::string_view name)
        -> std::unique_ptr<engine::component>;

inline auto
make_stereo_split_amplifier(std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return make_split_amplifier(2, name);
}

} // namespace piejam::audio::components
