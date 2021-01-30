// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/midi/fwd.h>

#include <memory>

namespace piejam::runtime::processors
{

auto make_midi_input_processor(std::unique_ptr<midi::input_processor>)
        -> std::unique_ptr<audio::engine::processor>;

} // namespace piejam::runtime::processors
