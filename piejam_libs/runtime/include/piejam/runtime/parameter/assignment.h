// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/fwd.h>

namespace piejam::runtime::parameter
{

template <class Value>
struct assignment
{
    parameter::key key;
    Value value;

    auto operator==(assignment const&) const noexcept -> bool = default;
};

} // namespace piejam::runtime::parameter
