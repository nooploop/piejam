// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/fx_chain_actions.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
toggle_fx_module_bypass::reduce(state& st) const
{
    st.fx_modules.update(fx_mod_id, [](fx::module& fx_mod) {
        fx_mod.bypassed = !fx_mod.bypassed;
    });
}

void
toggle_focused_fx_module_bypass::reduce(state& st) const
{
    st.fx_modules.update(
            st.gui_state.focused_fx_mod_id,
            [](fx::module& fx_mod) { fx_mod.bypassed = !fx_mod.bypassed; });
}

void
focus_fx_module::reduce(state& st) const
{
    st.gui_state.focused_fx_chain_id = fx_chain_id;
    st.gui_state.focused_fx_mod_id = fx_mod_id;
}

void
show_fx_module::reduce(state& st) const
{
    st.gui_state.root_view_mode_ = runtime::root_view_mode::fx_module;
}

} // namespace piejam::runtime::actions
