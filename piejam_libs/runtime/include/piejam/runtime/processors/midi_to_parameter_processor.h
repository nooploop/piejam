// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::runtime::processors
{

auto make_midi_cc_to_parameter_processor(float_parameter const&)
        -> std::unique_ptr<audio::engine::processor>;

auto make_midi_cc_to_parameter_processor(int_parameter const&)
        -> std::unique_ptr<audio::engine::processor>;

auto make_midi_cc_to_parameter_processor(bool_parameter const&)
        -> std::unique_ptr<audio::engine::processor>;

} // namespace piejam::runtime::processors
