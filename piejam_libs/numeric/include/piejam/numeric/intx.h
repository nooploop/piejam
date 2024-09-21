// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <type_traits>

namespace piejam::numeric
{

#pragma pack(push, 1)

template <class Integer, std::size_t Bits>
struct intx_t
{
    Integer value : Bits;

    constexpr intx_t() noexcept
        : value(0)
    {
    }

    template <class OtherInteger>
    constexpr intx_t(intx_t<OtherInteger, Bits> const& other) noexcept
        : value{static_cast<Integer>(other.value)}
    {
    }

    template <
            class T,
            std::enable_if_t<
                    std::is_floating_point_v<T> || std::is_integral_v<T>,
                    bool> = true>
    constexpr intx_t(T x)
        : value(x)
    {
    }

    template <class T>
    [[nodiscard]] constexpr explicit operator T() const noexcept
    {
        return static_cast<T>(value);
    }

    template <class T>
    [[nodiscard]] constexpr auto operator/(T const x) const noexcept
            -> std::enable_if_t<std::is_floating_point_v<T>, T>
    {
        return value / x;
    }

    [[nodiscard]] constexpr auto operator==(intx_t other) const noexcept -> bool
    {
        return value == other.value;
    }

    template <class ShiftInt>
    [[nodiscard]] constexpr auto operator<<(ShiftInt shift) const noexcept
            -> intx_t
    {
        return value << shift;
    }

    [[nodiscard]] constexpr auto operator^(intx_t other) const noexcept
            -> intx_t
    {
        return value ^ other.value;
    }
};

#pragma pack(pop)

using int24_t = intx_t<int, 24>;
using uint24_t = intx_t<unsigned, 24>;

} // namespace piejam::numeric
