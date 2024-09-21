// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/runtime/midi_assignment.h>

#include <memory>

namespace piejam::runtime::processors
{

auto make_midi_assignment_processor(midi_assignments_map const&)
        -> std::unique_ptr<audio::engine::processor>;

} // namespace piejam::runtime::processors
