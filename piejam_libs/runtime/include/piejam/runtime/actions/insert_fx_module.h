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

#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/unavailable_ladspa.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <span>
#include <vector>

namespace piejam::runtime::actions
{

struct insert_internal_fx_module final
    : ui::cloneable_action<insert_internal_fx_module, action>
    , visitable_engine_action<insert_internal_fx_module>
{
    mixer::bus_id fx_chain_bus;
    std::size_t position{};
    fx::internal type{};
    std::vector<fx::parameter_assignment> initial_assignments;

    auto reduce(state const&) const -> state override;
};

struct load_ladspa_fx_plugin final
    : ui::cloneable_action<load_ladspa_fx_plugin, action>
    , visitable_engine_action<load_ladspa_fx_plugin>
{
    mixer::bus_id fx_chain_bus;
    std::size_t position{};
    audio::ladspa::plugin_id_t plugin_id;
    std::string name;
    std::vector<fx::parameter_assignment> initial_assignments;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
