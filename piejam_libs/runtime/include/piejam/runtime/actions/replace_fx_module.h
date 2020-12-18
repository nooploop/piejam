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

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <string_view>

namespace piejam::runtime::actions
{

auto make_replace_fx_module_action(
        state const&,
        mixer::bus_id fx_chain_bus,
        std::size_t position,
        fx::internal) -> batch_action;

auto replace_fx_module(
        mixer::bus_id fx_chain_bus,
        std::size_t position,
        fx::internal) -> thunk_action;
auto replace_fx_module(std::size_t position, fx::internal) -> thunk_action;

auto make_replace_fx_module_action(
        state const&,
        mixer::bus_id fx_chain_bus,
        std::size_t position,
        audio::ladspa::plugin_id_t,
        std::string_view const& name) -> batch_action;

auto replace_fx_module(
        mixer::bus_id fx_chain_bus,
        std::size_t position,
        audio::ladspa::plugin_id_t,
        std::string_view const& name) -> thunk_action;
auto replace_fx_module(
        std::size_t position,
        audio::ladspa::plugin_id_t,
        std::string_view const& name) -> thunk_action;

} // namespace piejam::runtime::actions
