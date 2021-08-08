// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>
#include <memory>

namespace piejam::redux
{

template <class T>
concept cloneable = requires(T const& x)
{
    {
        x.clone()
        } -> std::same_as<std::unique_ptr<T>>;
};

} // namespace piejam::redux
