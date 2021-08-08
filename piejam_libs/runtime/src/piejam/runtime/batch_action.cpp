// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/ui/batch_action_impl.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::ui
{

template struct batch_action<state>;

} // namespace piejam::runtime::ui
