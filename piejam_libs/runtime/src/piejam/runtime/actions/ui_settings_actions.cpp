// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/ui_settings_actions.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
set_display_rotation::reduce(state& st) const
{
    st.display_rotation = display_rotation;
}

void
set_on_screen_keyboard_enabled::reduce(state& st) const
{
    st.on_screen_keyboard_enabled = enabled;
}

} // namespace piejam::runtime::actions
