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

#include <piejam/audio/types.h>
#include <piejam/runtime/parameters.h>

namespace piejam::runtime::actions
{

// actions

struct renotify;

struct device_action;
struct engine_action;

struct apply_app_config;

struct refresh_devices;
struct initiate_device_selection;

struct select_samplerate;
struct select_period_size;

struct select_bus_channel;
struct add_bus;
struct delete_bus;

template <audio::bus_direction>
struct set_bus_solo;
using set_input_bus_solo = set_bus_solo<audio::bus_direction::input>;

struct request_levels_update;
struct request_info_update;

struct set_bus_name;

template <class>
struct set_parameter_value;
using set_bool_parameter = set_parameter_value<bool_parameter>;
using set_float_parameter = set_parameter_value<float_parameter>;
using set_int_parameter = set_parameter_value<int_parameter>;

struct delete_fx_module;
struct insert_internal_fx_module;
struct finalize_ladspa_fx_plugin_scan;
struct load_ladspa_fx_plugin;

struct load_app_config;
struct save_app_config;
struct load_session;
struct save_session;

// visitors

struct device_action_visitor;
struct engine_action_visitor;

} // namespace piejam::runtime::actions
