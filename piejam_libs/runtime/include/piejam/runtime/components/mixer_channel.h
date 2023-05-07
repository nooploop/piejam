// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>
#include <string_view>

namespace piejam::runtime::components
{

auto make_mixer_channel_input(mixer::channel const&)
        -> std::unique_ptr<audio::engine::component>;

auto make_mixer_channel_output(
        mixer::channel const&,
        parameter_processor_factory&,
        audio::sample_rate) -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
