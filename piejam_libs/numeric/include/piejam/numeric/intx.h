// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <compare>
#include <concepts>

namespace piejam::numeric
{

#pragma pack(push, 1)

template <std::integral Integer, std::size_t Bits>
struct intx_t
{
    static_assert(sizeof(Integer) * 8 >= Bits);

    Integer value : Bits;

    constexpr intx_t() noexcept
        : value{0}
    {
    }

    template <std::integral OtherInteger>
    constexpr intx_t(intx_t<OtherInteger, Bits> const& other) noexcept
        : value{static_cast<Integer>(other.value)}
    {
    }

    template <class T>
        requires std::integral<T> || std::floating_point<T>
    constexpr intx_t(T x)
        : value{static_cast<Integer>(x)}
    {
    }

    template <class T>
    [[nodiscard]]
    constexpr explicit operator T() const noexcept
    {
        return static_cast<T>(value);
    }

    template <std::floating_point T>
    [[nodiscard]]
    constexpr auto operator/(T const x) const noexcept -> T
    {
        return value / x;
    }

    [[nodiscard]]
    constexpr auto operator<=>(intx_t const&) const noexcept = default;

    template <std::integral ShiftInt>
    [[nodiscard]]
    constexpr auto operator<<(ShiftInt shift) const noexcept -> intx_t
    {
        return value << shift;
    }

    [[nodiscard]]
    constexpr auto operator^(intx_t other) const noexcept -> intx_t
    {
        return value ^ other.value;
    }
};

#pragma pack(pop)

using int24_t = intx_t<int, 24>;
using uint24_t = intx_t<unsigned, 24>;

} // namespace piejam::numeric
