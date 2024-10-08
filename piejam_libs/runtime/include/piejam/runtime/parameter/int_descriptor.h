// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>

namespace piejam::runtime::parameter
{

struct int_descriptor
{
    using value_type = int;

    boxed_string name;

    int default_value{};

    int min{};
    int max{1};
};

} // namespace piejam::runtime::parameter
