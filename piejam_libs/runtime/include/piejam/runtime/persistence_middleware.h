// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/locations.h>

namespace piejam::runtime
{

class persistence_middleware
{
public:
    persistence_middleware(get_state_f, dispatch_f, next_f);

    void operator()(action const&);

private:
    template <class Action>
    void process_persistence_action(Action const&);

    get_state_f m_get_state;
    dispatch_f m_dispatch;
    next_f m_next;
};

} // namespace piejam::runtime
