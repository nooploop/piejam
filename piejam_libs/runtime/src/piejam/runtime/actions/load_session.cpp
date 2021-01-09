// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/load_session.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto load_session::reduce(state const& st) const -> state
{
    return st;
}

} // namespace piejam::runtime::actions
