// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::components
{

// audio in: 1
// audio out: 2
// event in: pan
auto make_pan(std::string_view name = {}) -> std::unique_ptr<engine::component>;

// audio in: 1
// audio out: 2
// event in: volume, pan
auto make_volume_pan(std::string_view name = {})
        -> std::unique_ptr<engine::component>;

// audio in: 2
// audio out: 2
// event in: balance
auto make_stereo_balance(std::string_view name = {})
        -> std::unique_ptr<engine::component>;

// audio in: 2
// audio out: 2
// event in: volume, balance
auto make_volume_stereo_balance(std::string_view name = {})
        -> std::unique_ptr<engine::component>;

} // namespace piejam::audio::components
