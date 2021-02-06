// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <boost/assert.hpp>

namespace piejam::runtime
{

class middleware_functors
{
public:
    middleware_functors(get_state_f get_state, dispatch_f dispatch, next_f next)
        : m_get_state(std::move(get_state))
        , m_dispatch(std::move(dispatch))
        , m_next(std::move(next))
    {
    }

    auto get_state() const -> state const&
    {
        BOOST_ASSERT(m_get_state);
        return m_get_state();
    }

    void dispatch(action const& a)
    {
        BOOST_ASSERT(m_dispatch);
        m_dispatch(a);
    }

    void next(action const& a)
    {
        BOOST_ASSERT(m_next);
        m_next(a);
    }

private:
    get_state_f m_get_state;
    dispatch_f m_dispatch;
    next_f m_next;
};

} // namespace piejam::runtime
