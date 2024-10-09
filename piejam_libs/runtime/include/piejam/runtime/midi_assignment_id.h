// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameters.h>

namespace piejam::runtime
{

using midi_assignment_id = parameter_id;

template <class ParamId>
using is_midi_assignable = boost::mp11::mp_contains<
        boost::mp11::mp_list<
                bool_parameter_id,
                float_parameter_id,
                int_parameter_id>,
        ParamId>;

template <class ParamId>
constexpr bool is_midi_assignable_v = is_midi_assignable<ParamId>::value;

auto is_valid_midi_assignment_id(midi_assignment_id const&) noexcept -> bool;

} // namespace piejam::runtime
