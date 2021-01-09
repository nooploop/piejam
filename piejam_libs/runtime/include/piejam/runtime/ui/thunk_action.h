// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/functors.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <functional>

namespace piejam::runtime::ui
{

template <class State>
struct thunk_action final : cloneable_action<thunk_action<State>, action<State>>
{
    using get_state_f = redux::get_state_f<State>;
    using dispatch_f = redux::dispatch_f<action<State>>;

    template <class Thunk>
    thunk_action(Thunk&& thunk)
        : m_thunk(std::forward<Thunk>(thunk))
    {
    }

    void operator()(get_state_f const&, dispatch_f const&) const;

    auto reduce(State const&) const -> State override;

private:
    std::function<void(get_state_f const&, dispatch_f const&)> m_thunk;
};

template <class State>
auto
as_thunk_action(action<State> const& a) -> thunk_action<State> const*
{
    return dynamic_cast<thunk_action<State> const*>(&a);
}

} // namespace piejam::runtime::ui
