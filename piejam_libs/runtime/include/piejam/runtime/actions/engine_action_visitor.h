// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
              request_info_update,
              load_ladspa_fx_plugin,
              move_fx_module_left,
              move_fx_module_right,
              start_midi_learning,
              stop_midi_learning,
              update_midi_assignments>
{
};

} // namespace piejam::runtime::actions
