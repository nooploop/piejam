// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::runtime::parameter
{

template <class Value>
struct generic_value
{
    using value_type = Value;

    Value default_value{};
};

} // namespace piejam::runtime::parameter
