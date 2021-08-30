// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
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
    constexpr biquad(coefficients const& c)
        : coeffs(c)
    {
    }
    constexpr biquad(coefficients&& c)
        : coeffs(std::move(c))
    {
    }

    constexpr auto process(T const x0) noexcept -> float
    {
        T const y0 = (coeffs.a0 * x0) + (coeffs.a1 * m_x1) +
                     (coeffs.a2 * m_x2) - (coeffs.b1 * m_y1) -
                     (coeffs.b2 * m_y2);
        m_x2 = m_x1;
        m_x1 = x0;
        m_y2 = m_y1;
        m_y1 = y0;
        return y0;
    }

private:
    T m_x1{};
    T m_x2{};
    T m_y1{};
    T m_y2{};
};

} // namespace piejam::audio::dsp
