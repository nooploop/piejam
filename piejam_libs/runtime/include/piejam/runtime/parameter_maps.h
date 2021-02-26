// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameters.h>

namespace piejam::runtime
{

using parameter_maps = parameter::maps_collection<
        float_parameter,
        bool_parameter,
        int_parameter,
        stereo_level_parameter>;

// workaround helpers to avoid some compiler issues on arm
auto get_parameter_value(parameter_maps const&, float_parameter_id)
        -> float const*;
auto get_parameter_value(parameter_maps const&, int_parameter_id) -> int const*;
auto get_parameter_value(parameter_maps const&, bool_parameter_id)
        -> bool const*;

} // namespace piejam::runtime
