// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

namespace piejam::runtime
{

class persistence_middleware
{
public:
    void operator()(middleware_functors const&, action const&);
};

} // namespace piejam::runtime
