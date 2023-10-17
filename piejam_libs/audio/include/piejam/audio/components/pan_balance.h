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
auto make_pan(
        std::unique_ptr<engine::processor> pan_converter,
        std::string_view name = {}) -> std::unique_ptr<engine::component>;

// audio in: 2
// audio out: 2
auto make_balance(
        std::unique_ptr<engine::processor> balance_converter,
        std::string_view name = {}) -> std::unique_ptr<engine::component>;

} // namespace piejam::audio::components
