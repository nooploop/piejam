// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

auto make_clip_processor(
        float min = -1.f,
        float max = 1.f,
        std::string_view name = {}) -> std::unique_ptr<processor>;

} // namespace piejam::audio::engine
