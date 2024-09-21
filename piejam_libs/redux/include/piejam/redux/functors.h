// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>

namespace piejam::redux
{

template <class State, class Action>
using reducer_f = std::function<void(State&, Action const&)>;

template <class State>
using subscriber_f = std::function<void(State const&)>;

template <class Action>
using dispatch_f = std::function<void(Action const&)>;

template <class Action>
using next_f = std::function<void(Action const&)>;

template <class State>
using get_state_f = std::function<State const&()>;

} // namespace piejam::redux
