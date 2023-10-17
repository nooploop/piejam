// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

// event in: pan
// event out: gain L, gain R
auto make_pan_processor(std::string_view name = {})
        -> std::unique_ptr<processor>;

// event in: volume, pan
// event out: gain L, gain R
auto make_volume_pan_processor(std::string_view name = {})
        -> std::unique_ptr<processor>;

// event in: balance
// event out: gain L, gain R
auto make_stereo_balance_processor(std::string_view name = {})
        -> std::unique_ptr<processor>;

// event in: volume, balance
// event out: gain L, gain R
auto make_volume_stereo_balance_processor(std::string_view name = {})
        -> std::unique_ptr<processor>;

} // namespace piejam::audio::engine
