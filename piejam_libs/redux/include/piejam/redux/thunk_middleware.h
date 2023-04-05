// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/functors.h>
#include <piejam/redux/make_middleware.h>
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
        using mw_fs_t = middleware_functors<State, Action>;
        if (auto* const ta = as_thunk_action(a))
        {
            (*ta)(std::bind_front(&mw_fs_t::get_state, mw_fs),
                  std::bind_front(&mw_fs_t::dispatch, mw_fs));
        }
        else
        {
            mw_fs.next(a);
        }
    }
};

} // namespace piejam::redux
