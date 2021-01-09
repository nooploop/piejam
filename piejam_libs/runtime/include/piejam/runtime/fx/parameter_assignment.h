// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

namespace piejam::runtime::fx
{

struct parameter_assignment
{
    parameter_key key;
    parameter_value value;
};

} // namespace piejam::runtime::fx
