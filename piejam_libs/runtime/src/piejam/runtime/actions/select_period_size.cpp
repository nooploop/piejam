// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/select_period_size.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
select_period_size::reduce(state const& st) const -> state
{
    auto new_st = st;
    new_st.period_size = period_sizes_from_state(st)[index];
    return new_st;
}

} // namespace piejam::runtime::actions
