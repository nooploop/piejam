// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/root_view_actions.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
set_root_view_mode::reduce(state& st) const
{
    st.gui_state.root_view_mode_ = mode;
}

void
show_fx_browser::reduce(state& st) const
{
    st.gui_state.fx_browser_fx_chain_id = fx_chain_id;
    st.gui_state.root_view_mode_ = root_view_mode::fx_browser;
}

} // namespace piejam::runtime::actions
