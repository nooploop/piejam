// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/math.h>

#include <numbers>

namespace piejam::audio
{

using stereo_gain = pair<float>;

inline constexpr auto
sinusoidal_constant_power_pan_exact(float pan_pos) -> stereo_gain
{
    constexpr float const pi_div_4 = std::numbers::pi_v<float> / 4.f;
    constexpr float const one_div_sqrt2 = 1.f / std::numbers::sqrt2_v<float>;
    float const cos_pan_pos = std::cos(pan_pos * pi_div_4);
    float const sin_pan_pos = std::sin(pan_pos * pi_div_4);
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
inline constexpr auto
sinusoidal_constant_power_pan(float pan_pos) -> stereo_gain
{
    constexpr float pi_f = std::numbers::pi_v<float>;
    constexpr float pi_sqr_f = pi_f * pi_f;
    constexpr float pi_cubed_f = pi_sqr_f * pi_f;
    constexpr float sqrt2_f = std::numbers::sqrt2_v<float>;
    constexpr float pi_div_4root2 = pi_f / (4.f * sqrt2_f);
    constexpr float pi_sqr_div_32root2 = pi_sqr_f / (32.f * sqrt2_f);
    constexpr float pi_cubed_div_384root2 = pi_cubed_f / (384.f * sqrt2_f);
    constexpr float pi_quad = pi_cubed_f * pi_f;
    constexpr float pi_quad_div_6144root2 = pi_quad / (6144.f * sqrt2_f);
    constexpr float inv_sqrt2_f = 1.f / sqrt2_f;
    float const x_sqr = pan_pos * pan_pos;
    float const x_cubed = x_sqr * pan_pos;
    float const x_quad = x_cubed * pan_pos;
    float const ax1 = pi_div_4root2 * pan_pos;
    float const ax2 = pi_sqr_div_32root2 * x_sqr;
    float const ax3 = pi_cubed_div_384root2 * x_cubed;
    float const ax4 = pi_quad_div_6144root2 * x_quad;
    float const axs = inv_sqrt2_f - ax2 + ax4;
    float const axt = ax1 - ax3;
    float const left = axs - axt;
    float const right = axs + axt;
    return {left, right};
}

inline constexpr auto
stereo_balance(float balance_pos) -> stereo_gain
{
    if (balance_pos < 0.f)
    {
        return stereo_gain{1.f, math::pow3(1 + balance_pos)};
    }
    else if (balance_pos > 0.f)
    {
        return stereo_gain{math::pow3(1 - balance_pos), 1.f};
    }
    else
    {
        return stereo_gain{1.f};
    }
}

} // namespace piejam::audio
