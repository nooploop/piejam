// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/flag_resetter.h>
#include <piejam/redux/functors.h>
#include <piejam/redux/middleware_functors.h>

namespace piejam::redux
{

class batch_middleware
{
public:
    batch_middleware(bool& batching)
        : m_batching(batching)
    {
    }

    template <class State, class Action>
    void operator()(
            middleware_functors<State, Action> const& mw_fs,
            Action const& a) const
    {
        if (auto* const batch = as_batch_action(a))
        {
            flag_resetter reset_batching{&m_batching};

            using std::begin;
            using std::end;

            for (auto it = begin(*batch), it_end = end(*batch); it != it_end;
                 ++it)
            {
                m_batching = std::next(it) != it_end;
                mw_fs.next(*it);
            }
        }
        else
        {
            mw_fs.next(a);
        }
    }

private:
    bool& m_batching;
};

} // namespace piejam::redux
