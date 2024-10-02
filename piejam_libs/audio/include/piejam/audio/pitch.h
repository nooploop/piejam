// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/math.h>

namespace piejam::audio
{

enum class pitchclass : int
{
    A,
    A_sharp,
    B,
    C,
    C_sharp,
    D,
    D_sharp,
    E,
    F,
    F_sharp,
    G,
    G_sharp
};

inline constexpr int semitones_per_octave = 12;
inline constexpr int cents_per_octave = 1200;

template <int Subdivisions, std::floating_point T>
[[nodiscard]]
constexpr auto
interval_from_frequency(T const f, T const f0)
{
    return Subdivisions * std::log2(f / f0);
}

template <int Subdivisions, std::floating_point T>
[[nodiscard]]
constexpr auto
frequency_from_interval(T const ival, T const f0)
{
    return std::pow(T{2}, ival / static_cast<T>(Subdivisions)) * f0;
}

struct pitch
{
    [[nodiscard]]
    static constexpr auto from_frequency(float frequency) noexcept -> pitch
    {
        BOOST_ASSERT(frequency > 0.f);

        pitch result;

        constexpr float A4_freq = 440.f;

        float const semitones_interval_f =
                interval_from_frequency<semitones_per_octave>(
                        frequency,
                        A4_freq);
        int const semitones_interval =
                static_cast<int>(std::round(semitones_interval_f));

        result.pitchclass_ = static_cast<pitchclass>(
                math::pos_mod(semitones_interval, semitones_per_octave));

        constexpr int base_octave = 4;
        constexpr int octave_start_offset = 9;
        constexpr int octaves_calc_offset = 8;

        result.octave =
                (semitones_interval + base_octave * semitones_per_octave +
                 octaves_calc_offset * semitones_per_octave +
                 octave_start_offset) /
                        semitones_per_octave -
                octaves_calc_offset;

        result.cents = interval_from_frequency<cents_per_octave>(
                frequency,
                frequency_from_interval<semitones_per_octave>(
                        static_cast<float>(semitones_interval),
                        A4_freq));

        return result;
    }

    pitchclass pitchclass_{pitchclass::A};
    int octave{4};
    float cents{};
};

} // namespace piejam::audio
