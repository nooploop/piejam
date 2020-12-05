// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/runtime/parameter/fwd.h>

namespace piejam::runtime
{

using float_parameter = parameter::float_;
using float_parameters = parameter::map<float_parameter>;
using float_parameter_id = parameter::id_t<float_parameter>;

using bool_parameter = parameter::bool_;
using bool_parameters = parameter::map<bool_parameter>;
using bool_parameter_id = parameter::id_t<bool_parameter>;

using int_parameter = parameter::int_;
using int_parameters = parameter::map<int_parameter>;
using int_parameter_id = parameter::id_t<int_parameter>;

using stereo_level_parameter = parameter::stereo_level;
using stereo_level_parameters = parameter::map<stereo_level_parameter>;
using stereo_level_parameter_id = parameter::id_t<stereo_level_parameter>;

} // namespace piejam::runtime
