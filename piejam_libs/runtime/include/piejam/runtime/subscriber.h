// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <piejam/fwd.h>
#include <piejam/reselect/fwd.h>

namespace piejam::runtime
{

using subscriber = reselect::subscriber<state>;

template <class Value>
using selector = reselect::selector<Value, state>;

using subscription_id = entity_id<struct subscription_tag>;
using subscriptions_manager = reselect::subscriptions_manager<subscription_id>;

} // namespace piejam::runtime
