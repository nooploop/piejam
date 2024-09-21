// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cmath>
#include <numbers>

namespace piejam::numeric::window
{

[[nodiscard]] constexpr auto
hamming(std::size_t const size, std::size_t const n) noexcept -> float
{
    constexpr float const two_pi = 2.f * std::numbers::pi_v<float>;
    float const two_pi_div_size_minus_1 = two_pi / (size - 1);
    return 0.54f - 0.46f * std::cos(two_pi_div_size_minus_1 * n);
}

template <std::size_t Size>
[[nodiscard]] constexpr auto
hamming_c(std::size_t const n) noexcept -> float
{
    return hamming(Size, n);
}

[[nodiscard]] constexpr auto
hann(std::size_t const size, std::size_t const n) noexcept -> float
{
    constexpr float const two_pi = 2.f * std::numbers::pi_v<float>;
    float const two_pi_div_size_minus_1 = two_pi / (size - 1);
    return 0.5f - 0.5f * std::cos(two_pi_div_size_minus_1 * n);
}

template <std::size_t Size>
[[nodiscard]] constexpr auto
hann_c(std::size_t const n) noexcept -> float
{
    return hann(Size, n);
}

} // namespace piejam::numeric::window
