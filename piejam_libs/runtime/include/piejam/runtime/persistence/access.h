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

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <filesystem>

namespace piejam::runtime::persistence
{

void load_app_config(locations const&, dispatch_f const&);
void save_app_config(locations const&, state const&);

void load_session(std::filesystem::path const&, dispatch_f const&);
void save_session(
        std::filesystem::path const&,
        state const&,
        fx::instance_plugin_id_map const&);

} // namespace piejam::runtime::persistence
