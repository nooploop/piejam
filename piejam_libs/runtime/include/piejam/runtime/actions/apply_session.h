// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/persistence/fwd.h>

namespace piejam::runtime::actions
{

void apply_session(persistence::session, dispatch_f const&);

} // namespace piejam::runtime::actions
