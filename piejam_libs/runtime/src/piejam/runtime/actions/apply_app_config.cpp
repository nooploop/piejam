// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/apply_app_config.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
apply_app_config::reduce(state& st) const
{
    st.rec_session = conf.rec_session;
}

} // namespace piejam::runtime::actions
