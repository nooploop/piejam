// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::components
{

auto make_stereo_level_meter(sample_rate, std::string_view name = "level_meter")
        -> std::unique_ptr<engine::component>;

} // namespace piejam::audio::components
