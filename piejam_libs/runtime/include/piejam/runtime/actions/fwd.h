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

namespace piejam::runtime::actions
{

// actions

struct device_action;
struct engine_action;

struct apply_app_config;

struct refresh_devices;
struct update_devices;
struct initiate_device_selection;
struct select_device;
struct select_samplerate;
struct select_period_size;
struct select_bus_channel;
struct add_device_bus;
struct delete_device_bus;

struct set_input_channel_volume;
struct set_input_channel_pan;
struct set_input_channel_mute;
struct set_input_solo;
struct set_output_channel_volume;
struct set_output_channel_balance;
struct set_output_channel_mute;
struct request_levels_update;
struct update_levels;
struct request_info_update;
struct update_info;

struct set_bus_name;

// visitors

struct device_action_visitor;
struct engine_action_visitor;

} // namespace piejam::runtime::actions
