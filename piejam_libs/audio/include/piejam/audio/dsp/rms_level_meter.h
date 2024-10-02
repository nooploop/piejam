// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sample_rate.h>

#include <piejam/functional/operators.h>
#include <piejam/math.h>

#include <mipp.h>

#include <boost/circular_buffer.hpp>

#include <chrono>
#include <cmath>
#include <concepts>

namespace piejam::audio::dsp
{

template <std::floating_point T = float>
class rms_level_meter
{
    static constexpr std::chrono::milliseconds default_rms_measure_time{800};
    static constexpr float default_min_level = 0.001f; // -60 dB

public:
    using value_type = mipp::Reg<std::remove_const_t<T>>;

    explicit rms_level_meter(
            sample_rate sr,
            std::chrono::milliseconds rms_measure_time =
                    default_rms_measure_time,
            T min_level = default_min_level)
        : m_min_level{min_level}
        , m_squared_history{
                  static_cast<std::size_t>(
                          std::chrono::duration_cast<std::chrono::duration<T>>(
                                  rms_measure_time)
                                  .count() *
                          sr.as_float<T>()) /
                          mipp::N<T>(),
                  0.f}
    {
    }

    void push_back(value_type const x)
    {
        T const sq = mipp::sum(x * x);
        m_squared_sum = m_squared_sum - m_squared_history.front() + sq;
        m_squared_history.push_back(sq);
    }

    [[nodiscard]]
    auto level() const noexcept -> T
    {
        return math::flush_to_zero_if(
                std::sqrt(std::abs(m_squared_sum) / m_squared_history_size),
                less<>(m_min_level));
    }

private:
    T m_min_level;

    boost::circular_buffer<T> m_squared_history;
    T m_squared_history_size{
            static_cast<T>(m_squared_history.size()) * mipp::N<T>()};
    T m_squared_sum{};
};

} // namespace piejam::audio::dsp
