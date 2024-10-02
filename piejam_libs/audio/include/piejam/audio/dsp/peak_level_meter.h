// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sample_rate.h>

#include <piejam/functional/operators.h>
#include <piejam/math.h>

#include <chrono>
#include <cmath>
#include <concepts>

namespace piejam::audio::dsp
{

template <std::floating_point T = float>
class peak_level_meter
{
    static constexpr std::chrono::milliseconds default_peak_decay_time{400};
    static constexpr T default_min_level = 0.001f; // -60 dB

public:
    using value_type = T;

    explicit peak_level_meter(
            sample_rate sr,
            std::chrono::milliseconds peak_decay_time = default_peak_decay_time,
            T min_level = default_min_level)
        : m_min_level{min_level}
        , m_g_release{std::pow(
                  9.f,
                  (-1.f / (std::chrono::duration_cast<std::chrono::duration<T>>(
                                   peak_decay_time)
                                   .count() *
                           sr.as_float<T>())))}
    {
    }

    void push_back(T const x)
    {
        auto const abs_x = std::abs(x);
        m_peak_level = abs_x > m_peak_level
                               ? abs_x
                               : abs_x + m_g_release * (m_peak_level - abs_x);
    }

    [[nodiscard]]
    auto level() const noexcept -> T
    {
        return math::flush_to_zero_if(m_peak_level, less<>(m_min_level));
    }

private:
    T m_min_level;
    T m_peak_level{};
    T m_g_release{};
};

} // namespace piejam::audio::dsp
