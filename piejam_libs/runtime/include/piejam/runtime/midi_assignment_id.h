// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameters.h>

#include <variant>

namespace piejam::runtime
{

using midi_assignment_id =
        std::variant<float_parameter_id, int_parameter_id, bool_parameter_id>;

auto is_valid_midi_assignment_id(midi_assignment_id const&) noexcept -> bool;

} // namespace piejam::runtime
