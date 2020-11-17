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

namespace piejam::runtime::actions
{

// actions

struct device_action;
struct engine_action;

struct apply_app_config;

struct refresh_devices;
struct update_devices;
struct initiate_device_selection;

template <audio::bus_direction>
struct select_device;
using select_input_device = select_device<audio::bus_direction::input>;
using select_output_device = select_device<audio::bus_direction::output>;

struct select_samplerate;
struct select_period_size;

struct select_bus_channel;
struct add_bus;
struct delete_bus;

template <audio::bus_direction>
struct set_bus_solo;
using set_input_bus_solo = set_bus_solo<audio::bus_direction::input>;

template <audio::bus_direction>
struct set_bus_pan_balance;
using set_input_bus_pan_balance =
        set_bus_pan_balance<audio::bus_direction::input>;
using set_output_bus_balance =
        set_bus_pan_balance<audio::bus_direction::output>;

template <audio::bus_direction>
struct set_bus_volume;
using set_input_bus_volume = set_bus_volume<audio::bus_direction::input>;
using set_output_bus_volume = set_bus_volume<audio::bus_direction::output>;

template <audio::bus_direction>
struct set_bus_mute;
using set_input_bus_mute = set_bus_mute<audio::bus_direction::input>;
using set_output_bus_mute = set_bus_mute<audio::bus_direction::output>;

struct request_levels_update;
struct update_levels;
struct request_info_update;
struct update_info;

template <audio::bus_direction>
struct set_bus_name;
using set_input_bus_name = set_bus_name<audio::bus_direction::input>;
using set_output_bus_name = set_bus_name<audio::bus_direction::output>;

// visitors

struct device_action_visitor;
struct engine_action_visitor;

} // namespace piejam::runtime::actions
