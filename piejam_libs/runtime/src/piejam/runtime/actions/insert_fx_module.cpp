// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/insert_fx_module.h>

#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
insert_internal_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    runtime::insert_internal_fx_module(
            new_st,
            fx_chain_id,
            position,
            type,
            initial_values,
            midi_assignments);

    return new_st;
}

auto
load_ladspa_fx_plugin::reduce(state const& st) const -> state
{
    return st;
}

auto
insert_ladspa_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    runtime::insert_ladspa_fx_module(
            new_st,
            fx_chain_id,
            position,
            instance_id,
            plugin_desc,
            control_inputs,
            initial_values,
            midi_assignments);

    return new_st;
}

auto
insert_missing_ladspa_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    runtime::insert_missing_ladspa_fx_module(
            new_st,
            fx_chain_id,
            position,
            unavailable_ladspa,
            name);

    return new_st;
}

} // namespace piejam::runtime::actions
