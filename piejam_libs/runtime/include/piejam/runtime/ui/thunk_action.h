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
struct thunk_action final : cloneable_action<thunk_action<State>, action>
{
    using get_state_f = redux::get_state_f<State>;
    using dispatch_f = redux::dispatch_f<action>;

    template <class Thunk>
    thunk_action(Thunk&& thunk)
        : m_thunk(std::forward<Thunk>(thunk))
    {
    }

    void operator()(get_state_f const&, dispatch_f const&) const;

private:
    std::function<void(get_state_f const&, dispatch_f const&)> m_thunk;
};

} // namespace piejam::runtime::ui
