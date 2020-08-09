// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/math.h>

#include <boost/math/constants/constants.hpp>

namespace piejam::audio
{

using stereo_gain = pair<float>;

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
    using namespace boost::math::float_constants;
    constexpr float pi_div_4root2 = pi / (4.f * root_two);
    constexpr float pi_sqr_div_32root2 = pi_sqr / (32.f * root_two);
    constexpr float pi_cubed_div_384root2 = pi_cubed / (384.f * root_two);
    constexpr float pi_quad = pi_cubed * pi;
    constexpr float pi_quad_div_6144root2 = pi_quad / (6144.f * root_two);
    float const x_sqr = pan_pos * pan_pos;
    float const x_cubed = x_sqr * pan_pos;
    float const x_quad = x_cubed * pan_pos;
    float const ax1 = pi_div_4root2 * pan_pos;
    float const ax2 = pi_sqr_div_32root2 * x_sqr;
    float const ax3 = pi_cubed_div_384root2 * x_cubed;
    float const ax4 = pi_quad_div_6144root2 * x_quad;
    float const axs = one_div_root_two - ax2 + ax4;
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
