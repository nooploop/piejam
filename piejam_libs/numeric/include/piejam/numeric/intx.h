// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <climits>
#include <cmath>
#include <compare>
#include <concepts>
#include <limits>

namespace piejam::numeric
{

#pragma pack(push, 1)

template <std::integral Integer, std::size_t Bits>
struct intx_t
{
    static_assert(sizeof(Integer) * CHAR_BIT > Bits);

    Integer value : Bits;

    constexpr intx_t() noexcept
        : value{0}
    {
    }

    template <std::integral OtherInteger>
    constexpr explicit intx_t(intx_t<OtherInteger, Bits> const& other) noexcept
        : value{static_cast<Integer>(other.value)}
    {
    }

    template <class T>
        requires std::integral<T> || std::floating_point<T>
    constexpr explicit intx_t(T x)
        : value{static_cast<Integer>(x)}
    {
    }

    template <class T>
        requires std::integral<T> || std::floating_point<T>
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
    constexpr auto operator==(intx_t const&) const noexcept -> bool = default;

    [[nodiscard]]
    constexpr auto operator<=>(intx_t const&) const noexcept = default;

    template <std::integral ShiftInt>
    [[nodiscard]]
    constexpr auto operator<<(ShiftInt shift) const noexcept -> intx_t
    {
        return intx_t{value << shift};
    }

    [[nodiscard]]
    constexpr auto operator^(intx_t other) const noexcept -> intx_t
    {
        return intx_t{value ^ other.value};
    }
};

#pragma pack(pop)

using int24_t = intx_t<int, 24>;
using uint24_t = intx_t<unsigned, 24>;

} // namespace piejam::numeric

template <class Integer, std::size_t Bits>
struct std::numeric_limits<piejam::numeric::intx_t<Integer, Bits>>
{
public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = std::is_signed_v<Integer>;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr std::float_round_style round_style =
            std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = std::numeric_limits<Integer>::is_modulo;
    static constexpr int digits =
            static_cast<int>(Bits) - std::is_signed_v<Integer>;
    static constexpr int digits10 = static_cast<int>(digits * std::log10(2));
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = std::numeric_limits<Integer>::traps;
    static constexpr bool tinyness_before = false;

    static constexpr auto
    min() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        if constexpr (std::is_signed_v<Integer>)
        {
            return piejam::numeric::intx_t<Integer, Bits>{
                    -(Integer{1} << (Bits - 1))};
        }
        else
        {
            return piejam::numeric::intx_t<Integer, Bits>{0};
        }
    }

    static constexpr auto
    lowest() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        return min();
    }

    static constexpr auto
    max() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        if constexpr (std::is_signed_v<Integer>)
        {
            return piejam::numeric::intx_t<Integer, Bits>{
                    (Integer{1} << (Bits - 1)) - 1};
        }
        else
        {
            return piejam::numeric::intx_t<Integer, Bits>{
                    (Integer{1} << Bits) - 1};
        }
    }

    static constexpr auto
    epsilon() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        return {};
    }

    static constexpr auto
    round_error() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        return {};
    }

    static constexpr auto
    infinity() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        return {};
    }

    static constexpr auto
    quiet_NaN() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        return {};
    }

    static constexpr auto
    signaling_NaN() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        return {};
    }

    static constexpr auto
    denorm_min() noexcept -> piejam::numeric::intx_t<Integer, Bits>
    {
        return {};
    }
};
