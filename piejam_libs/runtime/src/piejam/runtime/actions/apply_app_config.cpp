// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/apply_app_config.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
apply_app_config::reduce(state& st) const
{
    st.rec_session = conf.rec_session;
    st.display_rotation = conf.display_rotation;
    st.on_screen_keyboard_enabled = conf.on_screen_keyboard_enabled;
    st.startup_session = conf.startup_session;
    st.current_session = conf.last_session_file;
}

} // namespace piejam::runtime::actions
