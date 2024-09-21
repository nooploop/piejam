// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>
#include <utility>

namespace piejam::audio::dsp
{

template <std::floating_point T>
class biquad
{
public:
    struct coefficients
    {
        T b2{};
        T b1{};
        T a0{T{1}};
        T a1{};
        T a2{};
    } coeffs;

    constexpr biquad() noexcept = default;

    constexpr biquad(coefficients c) noexcept
        : coeffs{std::move(c)}
    {
    }

    // transposed canonical
    constexpr auto process(T const x) noexcept -> T
    {
        T const y = coeffs.a0 * x + m_z1;
        //
        m_z1 = coeffs.a1 * x - coeffs.b1 * y + m_z2;
        m_z2 = coeffs.a2 * x - coeffs.b2 * y;
        return y;
    }

private:
    T m_z1{};
    T m_z2{};
};

} // namespace piejam::audio::dsp
