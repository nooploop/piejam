// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/root_view_actions.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
set_root_view_mode::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.gui_state.root_view_mode_ = mode;

    return new_st;
}

auto
show_fx_browser::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.gui_state.fx_browser_fx_chain_id = fx_chain_id;
    new_st.gui_state.root_view_mode_ = root_view_mode::fx_browser;

    return new_st;
}

} // namespace piejam::runtime::actions
