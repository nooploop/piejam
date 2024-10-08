// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>

namespace piejam::runtime::parameter
{

struct bool_descriptor
{
    using value_type = bool;

    boxed_string name;

    value_type default_value{};
};

} // namespace piejam::runtime::parameter
