// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>
#include <memory>

namespace piejam::redux::concepts
{

// clang-format off
template <class T>
concept cloneable = requires(T const& x)
{
    { x.clone() } -> std::same_as<std::unique_ptr<T>>;
};
// clang-format on

template <class F, class State, class Action>
concept reducer = std::is_invocable_r_v<void, F, State&, Action const&>;

template <class F, class State>
concept subscriber = std::is_invocable_v<F, State const&>;

template <class F, class Action>
concept dispatch = std::is_invocable_v<F, Action const&>;

template <class F, class Action>
concept next = std::is_invocable_v<F, Action const&>;

template <class F, class State>
concept get_state = std::is_invocable_r_v<State const&, F>;

} // namespace piejam::redux::concepts
