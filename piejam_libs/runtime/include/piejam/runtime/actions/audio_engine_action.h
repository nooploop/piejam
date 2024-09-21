// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct audio_engine_action_visitor
    : ui::action_visitor_interface<
              set_external_audio_device_bus_channel,
              add_external_audio_device,
              remove_external_audio_device,
              add_mixer_channel,
              delete_mixer_channel,
              set_mixer_channel_route,
              enable_mixer_channel_aux_route,
              move_mixer_channel_left,
              move_mixer_channel_right,
              delete_fx_module,
              insert_internal_fx_module,
              insert_ladspa_fx_module,
              replace_missing_ladspa_fx_module,
              toggle_focused_fx_module_bypass,
              set_bool_parameter,
              set_float_parameter,
              set_int_parameter,
              request_parameters_update,
              request_info_update,
              request_streams_update,
              move_fx_module_up,
              move_fx_module_down,
              start_midi_learning,
              stop_midi_learning,
              update_midi_assignments,
              start_recording,
              stop_recording,
              apply_session>
{
};

struct audio_engine_action
    : ui::visitable_action_interface<audio_engine_action_visitor>
{
};

template <class Action>
using visitable_audio_engine_action =
        ui::visitable_action<Action, audio_engine_action>;

} // namespace piejam::runtime::actions
