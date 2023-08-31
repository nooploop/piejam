// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/insert_fx_module.h>

#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

void
insert_internal_fx_module::reduce(state& st) const
{
    auto fx_mod_id = runtime::insert_internal_fx_module(
            st,
            fx_chain_id,
            position,
            type,
            initial_values,
            midi_assignments);

    if (show_fx_module)
    {
        st.gui_state.focused_fx_chain_id = fx_chain_id;
        st.gui_state.focused_fx_mod_id = fx_mod_id;
        st.gui_state.root_view_mode_ = runtime::root_view_mode::fx_module;
    }
}

void
insert_ladspa_fx_module::reduce(state& st) const
{
    auto fx_mod_id = runtime::insert_ladspa_fx_module(
            st,
            fx_chain_id,
            position,
            instance_id,
            plugin_desc,
            control_inputs,
            initial_values,
            midi_assignments);

    if (show_fx_module)
    {
        st.gui_state.focused_fx_chain_id = fx_chain_id;
        st.gui_state.focused_fx_mod_id = fx_mod_id;
        st.gui_state.root_view_mode_ = runtime::root_view_mode::fx_module;
    }
}

void
insert_missing_ladspa_fx_module::reduce(state& st) const
{
    runtime::insert_missing_ladspa_fx_module(
            st,
            fx_chain_id,
            position,
            unavailable_ladspa,
            name);
}

} // namespace piejam::runtime::actions
