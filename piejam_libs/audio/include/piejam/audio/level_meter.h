// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/circular_buffer.hpp>

#include <cmath>
#include <numeric>

namespace piejam::audio
{

class level_meter
{
public:
    using value_type = float;

    level_meter(std::size_t decay_time, std::size_t rms_window_size)
        : m_decay_factor(1.f - 1.f / decay_time)
        , m_squared_history(rms_window_size, 0.f)
    {
    }

    void push_back(float const x)
    {
        m_peak_level = x > m_peak_level ? x : m_peak_level * m_decay_factor;

        m_dirty = true;
        float const sq = x * x;
        m_squared_sum += sq;
        m_squared_sum -= m_squared_history.front();
        m_squared_history.push_back(sq);
    }

    [[nodiscard]] auto get() const noexcept -> float
    {
        return m_peak_level;
    }

    [[nodiscard]] auto get_rms() const noexcept -> float
    {
        return std::sqrt(m_squared_sum / m_squared_history.size());
    }

    void clear() noexcept
    {
        if (m_dirty)
        {
            m_peak_level = 0.f;
            m_squared_history.assign(m_squared_history.size(), 0.f);
            m_squared_sum = 0.f;
            m_dirty = false;
        }
    }

private:
    float m_peak_level{};
    float m_decay_factor;
    boost::circular_buffer<float> m_squared_history;
    float m_squared_sum{};
    bool m_dirty{};
};

} // namespace piejam::audio
