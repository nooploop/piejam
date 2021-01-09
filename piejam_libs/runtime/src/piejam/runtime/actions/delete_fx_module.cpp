// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/delete_fx_module.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
delete_fx_module::reduce(state const& st) const -> state
{
    auto new_st = st;

    remove_fx_module(new_st, fx_mod_id);

    return new_st;
}

} // namespace piejam::runtime::actions
