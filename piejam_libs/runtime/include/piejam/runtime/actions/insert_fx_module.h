// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/unavailable_ladspa.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
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
    std::vector<fx::parameter_value_assignment> initial_values;
    std::vector<fx::parameter_midi_assignment> midi_assigns;

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
    std::vector<fx::parameter_value_assignment> initial_values;
    std::vector<fx::parameter_midi_assignment> midi_assigns;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
