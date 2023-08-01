// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/fx_chain_actions.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
toggle_fx_module_bypass::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.fx_modules.update(fx_mod_id, [](fx::module& fx_mod) {
        fx_mod.bypassed = !fx_mod.bypassed;
    });

    return new_st;
}

auto
toggle_focused_fx_module_bypass::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.fx_modules.update(
            st.gui_state.focused_fx_mod_id,
            [](fx::module& fx_mod) { fx_mod.bypassed = !fx_mod.bypassed; });

    return new_st;
}

auto
focus_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.gui_state.focused_fx_chain_id = fx_chain_id;
    new_st.gui_state.focused_fx_mod_id = fx_mod_id;

    return new_st;
}

auto
show_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.gui_state.root_view_mode_ = runtime::root_view_mode::fx_module;

    return new_st;
}

} // namespace piejam::runtime::actions
