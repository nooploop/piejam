// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/request_parameters_update.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
request_parameters_update::reduce(state const& st) const -> state
{
    return st;
}

} // namespace piejam::runtime::actions
