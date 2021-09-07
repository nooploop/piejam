// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/midi_assignment_id.h>

#include <piejam/entity_id.h>

namespace piejam::runtime
{

auto
is_valid_midi_assignment_id(midi_assignment_id const& id) noexcept -> bool
{
    return std::visit([](auto const& pid) { return pid.valid(); }, id);
}

} // namespace piejam::runtime
