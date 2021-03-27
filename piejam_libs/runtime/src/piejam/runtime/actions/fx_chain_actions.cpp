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

} // namespace piejam::runtime::actions
