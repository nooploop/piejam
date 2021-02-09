// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameters.h>

namespace piejam::runtime
{

using parameter_maps =
        boost::mp11::mp_rename<parameters_t, parameter::maps_collection>;

} // namespace piejam::runtime
