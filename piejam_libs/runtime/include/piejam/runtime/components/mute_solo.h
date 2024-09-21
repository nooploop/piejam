// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>
#include <string_view>

namespace piejam::runtime::components
{

auto make_mute_solo(std::string_view name = {})
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
