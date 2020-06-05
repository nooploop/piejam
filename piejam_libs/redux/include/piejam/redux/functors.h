// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <functional>
#include <memory>

namespace piejam::redux
{

template <class State, class Action>
using reducer_f = std::function<State(State const&, Action const&)>;

template <class State>
using subscriber_f = std::function<void(State const&)>;

template <class Action>
using dispatch_f = std::function<void(std::unique_ptr<Action>)>;

template <class Action>
using next_f = std::function<void(Action const&)>;

template <class State>
using get_state_f = std::function<State const&()>;

} // namespace piejam::redux
