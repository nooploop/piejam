// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cmath>
#include <numbers>
#include <span>

namespace piejam::audio::dsp
{

template <std::floating_point T>
constexpr void
generate_sine(
        std::span<T> const out,
        T const sr,
        T const freq,
        T const amp = T{1},
        T const phi = T{0})
{
    T const two_pi_freq_div_sr = 2.f * std::numbers::pi_v<T> * freq / sr;
    for (std::size_t n = 0, e = out.size(); n < e; ++n)
    {
        out[n] = amp * std::sin(n * two_pi_freq_div_sr + phi);
    }
}

} // namespace piejam::audio::dsp
