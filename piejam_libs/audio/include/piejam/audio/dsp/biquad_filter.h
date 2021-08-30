// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/dsp/biquad.h>
#include <piejam/math.h>

#include <cmath>
#include <numbers>

namespace piejam::audio::dsp::biquad_filter
{

template <std::floating_point T>
using coefficients = typename biquad<T>::coefficients;

template <std::floating_point T>
constexpr auto
calc_phi(T const freq_c, T const inv_sr) noexcept -> T
{
    return T{2} * std::numbers::pi_v<T> * freq_c * inv_sr;
}

template <std::floating_point T>
constexpr auto
calc_Q(T const res, T const min_Q, T const max_Q) noexcept -> T
{
    return math::linear_map(res, T{0}, T{1}, min_Q, max_Q);
}

template <std::floating_point T>
constexpr auto
calc_Q_lp_hp(T const res) noexcept -> T
{
    return calc_Q(res, std::sqrt(T{.5f}), T{10});
}

template <std::floating_point T>
constexpr auto
calc_min_Q_bp_br(T const cutoff, T const inv_sr) noexcept -> T
{
    return cutoff * T{4} * inv_sr;
}

template <std::floating_point T>
constexpr auto
calc_Q_bp(T const cutoff, T const res, T const inv_sr) noexcept -> T
{
    return calc_Q(res, calc_min_Q_bp_br(cutoff, inv_sr), T{25});
}

template <std::floating_point T>
constexpr auto
calc_Q_br(T const cutoff, T const res, T const inv_sr) noexcept -> T
{
    return calc_Q(res, calc_min_Q_bp_br(cutoff, inv_sr), T{4});
}

template <std::floating_point T>
constexpr auto
calc_b2_lp_hp(T const phi, T const Q) noexcept -> T
{
    T const two_Q = T{2} * Q;
    T const sin_phi = std::sin(phi);
    return (two_Q - sin_phi) / (two_Q + sin_phi);
}

template <std::floating_point T>
constexpr auto
calc_b2_bp_br(T const phi, T const Q) noexcept -> T
{
    constexpr T const pi_div_four = std::numbers::pi_v<T> / T{4};
    return std::tan(pi_div_four - phi / (T{2} * Q));
}

template <std::floating_point T>
constexpr auto
calc_b1(T const phi, T const b2) noexcept -> T
{
    return -(T{1} + b2) * std::cos(phi);
}

template <std::floating_point T>
constexpr auto
make_lp_coefficients(T const cutoff, T const res, T const inv_sr) noexcept
        -> coefficients<T>
{
    T const phi = calc_phi(cutoff, inv_sr);
    T const Q = calc_Q_lp_hp(res);
    T const b2 = calc_b2_lp_hp(phi, Q);
    T const b1 = calc_b1(phi, b2);
    T const a0 = .25f * (1.f + b1 + b2);
    return coefficients<T>{
            .b2 = b2,
            .b1 = b1,
            .a0 = a0,
            .a1 = 2.f * a0,
            .a2 = a0};
}

template <std::floating_point T>
constexpr auto
make_bp_coefficients(T const cutoff, T const res, T const inv_sr) noexcept
        -> coefficients<T>
{
    T const phi = calc_phi(cutoff, inv_sr);
    T const Q = calc_Q_bp(cutoff, res, inv_sr);
    T const b2 = calc_b2_bp_br(phi, Q);
    T const b1 = calc_b1(phi, b2);
    T const a0 = .5f * (1.f - b2);
    return coefficients<T>{.b2 = b2, .b1 = b1, .a0 = a0, .a1 = 0.f, .a2 = -a0};
}

template <std::floating_point T>
constexpr auto
make_hp_coefficients(T const cutoff, T const res, T const inv_sr) noexcept
        -> coefficients<T>
{
    T const phi = calc_phi(cutoff, inv_sr);
    T const Q = calc_Q_lp_hp(res);
    T const b2 = calc_b2_lp_hp(phi, Q);
    T const b1 = calc_b1(phi, b2);
    T const a0 = .25f * (1.f - b1 + b2);
    return coefficients<T>{
            .b2 = b2,
            .b1 = b1,
            .a0 = a0,
            .a1 = -2.f * a0,
            .a2 = a0};
}

template <std::floating_point T>
constexpr auto
make_br_coefficients(T const cutoff, T const res, T const inv_sr) noexcept
        -> coefficients<T>
{
    T const phi = calc_phi(cutoff, inv_sr);
    T const Q = calc_Q_br(cutoff, res, inv_sr);
    T const b2 = calc_b2_bp_br(phi, Q);
    T const b1 = calc_b1(phi, b2);
    T const a0 = .5f * (1.f + b2);
    return coefficients<T>{.b2 = b2, .b1 = b1, .a0 = a0, .a1 = b1, .a2 = a0};
}

} // namespace piejam::audio::dsp::biquad_filter
