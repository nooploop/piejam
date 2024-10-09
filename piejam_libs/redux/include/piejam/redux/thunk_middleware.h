// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/functors.h>
#include <piejam/redux/middleware_functors.h>

namespace piejam::redux
{

class thunk_middleware
{
public:
    template <class State, class Action>
    void operator()(
            middleware_functors<State, Action> const& mw_fs,
            Action const& a) const
    {
        if (auto* const ta = as_thunk_action(a))
        {
            (*ta)(mw_fs.get_state_f(), mw_fs.dispatch_f());
        }
        else
        {
            mw_fs.next(a);
        }
    }
};

} // namespace piejam::redux
