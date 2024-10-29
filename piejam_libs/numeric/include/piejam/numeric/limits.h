// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/numeric/intx.h>

#include <limits>

namespace piejam::numeric
{

template <class T>
struct limits
{
    [[nodiscard]]
    static constexpr auto min() noexcept -> T
    {
        return std::numeric_limits<T>::min();
    }

    [[nodiscard]]
    static constexpr auto max() noexcept -> T
    {
        return std::numeric_limits<T>::max();
    }
};

template <>
struct limits<int24_t>
{
    [[nodiscard]]
    static constexpr auto min() noexcept -> int24_t
    {
        return int24_t{-8388608};
    }

    [[nodiscard]]
    static constexpr auto max() noexcept -> int24_t
    {
        return int24_t{8388607};
    }
};

template <>
struct limits<uint24_t>
{
    [[nodiscard]]
    static constexpr auto min() noexcept -> uint24_t
    {
        return uint24_t{0};
    }

    [[nodiscard]]
    static constexpr auto max() noexcept -> uint24_t
    {
        return uint24_t{16777215};
    }
};

} // namespace piejam::numeric
