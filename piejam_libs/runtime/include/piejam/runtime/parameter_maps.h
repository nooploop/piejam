// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameters.h>

namespace piejam::runtime
{

using parameter_maps = parameter::maps_collection<
        parameter::float_,
        parameter::int_,
        parameter::bool_,
        parameter::stereo_level>;

} // namespace piejam::runtime
