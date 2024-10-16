// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/numeric/pow_n.h>

#include <cmath>
#include <numbers>

namespace piejam::audio::dsp
{

template <std::floating_point T>
constexpr auto
sinusoidal_constant_power_pan_exact(T pan_pos) -> pair<T>
{
    constexpr T pi_div_4 = std::numbers::pi_v<T> / T{4};
    constexpr T one_div_sqrt2 = T{1} / std::numbers::sqrt2_v<T>;
    T const cos_pan_pos = std::cos(pan_pos * pi_div_4);
    T const sin_pan_pos = std::sin(pan_pos * pi_div_4);
    return {one_div_sqrt2 * (cos_pan_pos - sin_pan_pos),
            one_div_sqrt2 * (cos_pan_pos + sin_pan_pos)};
}

// left = 1/sqrt(2) (cos(x×π/4) - sin(x×π/4))
//     ~= 1/sqrt(2) -
//           (π x)/(4 sqrt(2)) -
//           (π^2 x^2)/(32 sqrt(2)) +
//           (π^3 x^3)/(384 sqrt(2)) +
//           (π^4 x^4)/(6144 sqrt(2))
// right = 1/sqrt(2) (cos(x×π/4) + sin(x×π/4))
//      ~= 1/sqrt(2) +
//           (π x)/(4 sqrt(2)) -
//           (π^2 x^2)/(32 sqrt(2)) -
//           (π^3 x^3)/(384 sqrt(2)) +
//           (π^4 x^4)/(6144 sqrt(2))
template <std::floating_point T>
constexpr auto
sinusoidal_constant_power_pan(T pan_pos) -> pair<T>
{
    constexpr T pi = std::numbers::pi_v<T>;
    constexpr T sqrt2 = std::numbers::sqrt2_v<T>;
    constexpr T pi_div_4root2 = pi / (T{4} * sqrt2);
    constexpr T pi_sqr = pi * pi;
    constexpr T pi_sqr_div_32root2 = pi_sqr / (T{32} * sqrt2);
    constexpr T pi_cubed = pi_sqr * pi;
    constexpr T pi_cubed_div_384root2 = pi_cubed / (T{384} * sqrt2);
    constexpr T pi_quad = pi_cubed * pi;
    constexpr T pi_quad_div_6144root2 = pi_quad / (T{6144} * sqrt2);
    constexpr T inv_sqrt2_f = T{1} / sqrt2;
    T const x_sqr = pan_pos * pan_pos;
    T const x_cubed = x_sqr * pan_pos;
    T const x_quad = x_cubed * pan_pos;
    T const ax1 = pi_div_4root2 * pan_pos;
    T const ax2 = pi_sqr_div_32root2 * x_sqr;
    T const ax3 = pi_cubed_div_384root2 * x_cubed;
    T const ax4 = pi_quad_div_6144root2 * x_quad;
    T const axs = inv_sqrt2_f - ax2 + ax4;
    T const axt = ax1 - ax3;
    T const left = axs - axt;
    T const right = axs + axt;
    return {left, right};
}

template <std::floating_point T>
constexpr auto
stereo_balance(T balance_pos) -> pair<T>
{
    if (balance_pos < T{0})
    {
        return {T{1}, numeric::pow_n<3>(T{1} + balance_pos)};
    }

    if (balance_pos > 0.f)
    {
        return {numeric::pow_n<3>(T{1} - balance_pos), T{1}};
    }

    return pair{T{1}};
}

} // namespace piejam::audio::dsp
