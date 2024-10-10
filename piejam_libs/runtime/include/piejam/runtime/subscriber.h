// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <piejam/fwd.h>
#include <piejam/redux/fwd.h>

namespace piejam::runtime
{

using subscriber = redux::subscriber<state>;

template <class Value>
using selector = redux::selector<Value, state>;

using subscription_id = entity_id<struct subscription_tag>;
using subscriptions_manager = redux::subscriptions_manager<subscription_id>;

} // namespace piejam::runtime
