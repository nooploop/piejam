// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::runtime::parameter
{

struct bool_descriptor
{
    using value_type = bool;

    value_type default_value{};
};

} // namespace piejam::runtime::parameter
