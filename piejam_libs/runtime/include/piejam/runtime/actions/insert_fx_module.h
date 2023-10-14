// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/ladspa/plugin_descriptor.h>
#include <piejam/ladspa/port_descriptor.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/actions/ladspa_fx_action.h>
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
    : ui::cloneable_action<insert_internal_fx_module, reducible_action>
    , visitable_engine_action<insert_internal_fx_module>
{
    mixer::channel_id fx_chain_id;
    std::size_t position{};
    fx::internal_id type{};
    std::vector<fx::parameter_value_assignment> initial_values;
    std::vector<fx::parameter_midi_assignment> midi_assignments;
    bool show_fx_module{};

    void reduce(state&) const override;
};

struct load_ladspa_fx_plugin final
    : ui::cloneable_action<load_ladspa_fx_plugin, action>
    , visitable_ladspa_fx_action<load_ladspa_fx_plugin>
{
    mixer::channel_id fx_chain_id;
    std::size_t position{};
    ladspa::plugin_id_t plugin_id;
    std::string name;
    std::vector<fx::parameter_value_assignment> initial_values;
    std::vector<fx::parameter_midi_assignment> midi_assignments;
    bool show_fx_module{};
};

struct insert_ladspa_fx_module final
    : ui::cloneable_action<insert_ladspa_fx_module, reducible_action>
    , visitable_engine_action<insert_ladspa_fx_module>
{
    mixer::channel_id fx_chain_id;
    std::size_t position{};
    ladspa::instance_id instance_id;
    ladspa::plugin_descriptor plugin_desc;
    std::span<ladspa::port_descriptor const> control_inputs;
    std::vector<fx::parameter_value_assignment> initial_values;
    std::vector<fx::parameter_midi_assignment> midi_assignments;
    bool show_fx_module{};

    void reduce(state&) const override;
};

struct insert_missing_ladspa_fx_module final
    : ui::cloneable_action<insert_missing_ladspa_fx_module, reducible_action>
{
    mixer::channel_id fx_chain_id;
    std::size_t position{};
    fx::unavailable_ladspa unavailable_ladspa;
    std::string name;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
