// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::runtime::parameter
{

struct int_
{
    using value_type = int;

    int default_value{};

    int min{};
    int max{1};
};

} // namespace piejam::runtime::parameter
