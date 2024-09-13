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
struct recorder_action;

struct renotify;

struct apply_app_config;

struct refresh_devices;
struct initiate_device_selection;
struct refresh_midi_devices;

struct select_sample_rate;
struct select_period_size;
struct select_period_count;

struct activate_midi_device;
struct deactivate_midi_device;

struct set_external_audio_device_bus_channel;
struct add_external_audio_device;
struct remove_external_audio_device;

struct add_mixer_channel;
struct delete_mixer_channel;
struct set_mixer_channel_route;
struct move_mixer_channel_left;
struct move_mixer_channel_right;
struct show_fx_browser;

struct request_parameters_update;
struct request_info_update;
struct request_streams_update;

struct update_streams;

struct set_external_audio_device_name;

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
struct move_fx_module_up;
struct move_fx_module_down;
struct toggle_fx_module_bypass;
struct toggle_focused_fx_module_bypass;

struct start_midi_learning;
struct stop_midi_learning;
struct update_midi_assignments;

struct load_app_config;
struct save_app_config;
struct load_session;
struct save_session;

struct start_recording;
struct stop_recording;

// visitors

struct device_action_visitor;
struct engine_action_visitor;
struct midi_control_action_visitor;
struct persistence_action_visitor;
struct recorder_action_visitor;

} // namespace piejam::runtime::actions
