// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

namespace piejam::runtime::fx
{

template <class Value>
struct parameter_assignment
{
    parameter_key key;
    Value value;

    auto operator==(parameter_assignment const&) const noexcept
            -> bool = default;
};

} // namespace piejam::runtime::fx
