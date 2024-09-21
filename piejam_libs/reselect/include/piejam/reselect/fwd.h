// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace piejam::reselect
{

using selector_id_t = std::size_t;

template <class Value, class State>
class selector;

template <class State>
class subscriber;

template <class Id>
class subscriptions_manager;

} // namespace piejam::reselect
