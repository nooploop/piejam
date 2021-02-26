// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/io_direction.h>
#include <piejam/runtime/parameters.h>

namespace piejam::runtime::actions
{

// actions

struct device_action;
struct engine_action;
struct midi_control_action;
struct persistence_action;

struct renotify;

struct apply_app_config;

struct refresh_devices;
struct initiate_device_selection;
struct refresh_midi_devices;

struct select_samplerate;
struct select_period_size;

struct activate_midi_device;
struct deactivate_midi_device;

struct select_bus_channel;
struct add_bus;
struct delete_bus;

struct add_mixer_bus;
struct delete_mixer_bus;
template <io_direction>
struct set_mixer_channel_route;
using set_mixer_channel_input = set_mixer_channel_route<io_direction::input>;
using set_mixer_channel_output = set_mixer_channel_route<io_direction::output>;

template <io_direction>
struct set_bus_solo;
using set_input_bus_solo = set_bus_solo<io_direction::input>;
using set_output_bus_solo = set_bus_solo<io_direction::output>;

struct request_parameters_update;
struct request_info_update;

struct set_device_bus_name;

template <class>
struct set_parameter_value;
using set_bool_parameter = set_parameter_value<bool_parameter>;
using set_float_parameter = set_parameter_value<float_parameter>;
using set_int_parameter = set_parameter_value<int_parameter>;

struct delete_fx_module;
struct insert_internal_fx_module;
struct finalize_ladspa_fx_plugin_scan;
struct load_ladspa_fx_plugin;
struct insert_ladspa_fx_module;
struct insert_missing_ladspa_fx_module;
struct move_fx_module_left;
struct move_fx_module_right;

struct start_midi_learning;
struct stop_midi_learning;
struct update_midi_assignments;

struct load_app_config;
struct save_app_config;
struct load_session;
struct save_session;

// visitors

struct device_action_visitor;
struct engine_action_visitor;
struct midi_control_action_visitor;
struct persistence_action_visitor;

} // namespace piejam::runtime::actions
