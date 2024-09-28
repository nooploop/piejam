// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sample_rate.h>

#include <boost/circular_buffer.hpp>

#include <cmath>

namespace piejam::audio::dsp
{

class level_meter
{
    static constexpr float const peak_decay_time = 0.4f;     // in sec
    static constexpr float const rms_measure_time = 0.8f;    // in sec
    static constexpr float const default_min_level = 0.001f; // -60 dB

public:
    using value_type = float;

    explicit level_meter(sample_rate sr, float min_level = default_min_level)
        : m_min_level{min_level}
        , m_g_release{std::pow(9.f, (-1.f / (peak_decay_time * sr.as_float())))}
        , m_squared_history{
                  static_cast<std::size_t>(rms_measure_time * sr.as_float()),
                  0.f}
    {
    }

    void push_back(float const x)
    {
        auto const abs_x = std::abs(x);
        m_peak_level = abs_x > m_peak_level
                               ? abs_x
                               : abs_x + m_g_release * (m_peak_level - abs_x);

        double const xl = x;
        double const sq = xl * xl;
        m_squared_sum = m_squared_sum - m_squared_history.front() + sq;
        m_squared_history.push_back(sq);
    }

    [[nodiscard]]
    auto peak_level() const noexcept -> float
    {
        return clamp_min_to_zero(m_peak_level);
    }

    [[nodiscard]]
    auto rms_level() const noexcept -> float
    {
        double const squared_sum = std::max(m_squared_sum, 0.);
        return clamp_min_to_zero(static_cast<float>(std::sqrt(
                squared_sum / static_cast<double>(m_squared_history.size()))));
    }

private:
    [[nodiscard]]
    auto clamp_min_to_zero(float lvl) const -> float
    {
        return static_cast<float>(m_min_level <= lvl) * lvl;
    }

    float m_min_level;
    float m_peak_level{};
    float m_g_release{};

    boost::circular_buffer<double> m_squared_history;
    double m_squared_sum{};
};

} // namespace piejam::audio::dsp
