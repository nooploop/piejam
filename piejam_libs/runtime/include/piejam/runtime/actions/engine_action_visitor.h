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

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct engine_action_visitor
    : ui::action_visitor_interface<
              select_bus_channel,
              add_bus,
              delete_bus,
              delete_fx_module,
              insert_internal_fx_module,
              set_bool_parameter,
              set_float_parameter,
              set_int_parameter,
              set_input_bus_solo,
              request_levels_update,
              update_levels,
              request_info_update,
              update_info,
              load_ladspa_fx_plugin>
{
};

} // namespace piejam::runtime::actions
