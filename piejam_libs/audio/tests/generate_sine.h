// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/indices.h>

#include <cmath>
#include <numbers>
#include <span>

namespace piejam::audio::dsp::test
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
    for (auto n : range::indices(out))
    {
        out[n] = amp * std::sin(n * two_pi_freq_div_sr + phi);
    }
}

} // namespace piejam::audio::dsp::test
