// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>

namespace piejam::runtime::components
{

auto make_mixer_channel_input(mixer::channel const&)
        -> std::unique_ptr<audio::engine::component>;

auto make_mixer_channel_output(
        mixer::channel const&,
        parameter_processor_factory&,
        processors::stream_processor_factory&,
        audio::sample_rate) -> std::unique_ptr<audio::engine::component>;

auto make_mixer_channel_aux_send(
        mixer::channel const&,
        float_parameter_id aux_volume,
        parameter_processor_factory&)
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
