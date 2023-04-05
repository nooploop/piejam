// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

namespace piejam::runtime
{

class persistence_middleware
{
public:
    void operator()(middleware_functors const&, action const&);

private:
    template <class Action>
    void process_persistence_action(middleware_functors const&, Action const&);
};

} // namespace piejam::runtime
