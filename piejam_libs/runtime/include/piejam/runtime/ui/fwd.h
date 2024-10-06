// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/functors.h>

namespace piejam::runtime::ui
{

struct action;

template <class State>
struct reducible_action;

template <class DerivedAction, class ActionInterface>
struct cloneable_action;

template <class State>
using get_state_f = redux::get_state_f<State>;

using dispatch_f = redux::dispatch_f<action>;

using next_f = redux::next_f<action>;

template <class State>
struct thunk_action;

template <class State>
struct update_state_action;

} // namespace piejam::runtime::ui
