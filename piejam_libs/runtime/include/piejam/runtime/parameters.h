// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/fwd.h>

#include <boost/mp11/list.hpp>

namespace piejam::runtime
{

using float_parameter = parameter::float_;
using float_parameter_id = parameter::id_t<float_parameter>;

using bool_parameter = parameter::bool_;
using bool_parameter_id = parameter::id_t<bool_parameter>;

using int_parameter = parameter::int_;
using int_parameter_id = parameter::id_t<int_parameter>;

using stereo_level_parameter = parameter::stereo_level;
using stereo_level_parameter_id = parameter::id_t<stereo_level_parameter>;

using parameters_t = boost::mp11::mp_list<
        float_parameter,
        bool_parameter,
        int_parameter,
        stereo_level_parameter>;

} // namespace piejam::runtime
