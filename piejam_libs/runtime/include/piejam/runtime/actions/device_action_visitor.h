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

namespace piejam::runtime::actions
{

struct device_action_visitor
    : ui::action_visitor_interface<
              apply_app_config,
              refresh_devices,
              update_devices,
              initiate_device_selection,
              select_device,
              select_samplerate,
              select_period_size,
              select_bus_channel,
              add_device_bus,
              delete_device_bus>
{
};

} // namespace piejam::runtime::actions
