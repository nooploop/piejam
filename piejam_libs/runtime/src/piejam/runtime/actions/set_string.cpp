// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_string.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
set_string::reduce(state& st) const
{
    st.strings.set(id, str);
}

} // namespace piejam::runtime::actions
