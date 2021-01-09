// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/select_fx_chain_bus.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
select_fx_chain_bus::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.fx_chain_bus = bus_id;

    return new_st;
}

} // namespace piejam::runtime::actions
