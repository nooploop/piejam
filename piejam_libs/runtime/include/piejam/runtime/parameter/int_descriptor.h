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
    using value_to_string_fn = std::string (*)(value_type);

    boxed_string name;

    int default_value;

    int min;
    int max;

    value_to_string_fn value_to_string{
            [](value_type x) { return std::to_string(x); }};
};

} // namespace piejam::runtime::parameter
