// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <vector>

namespace piejam::audio::dsp
{

// Based on Algorithm 8.6 from "Audio Processes" by David Creasy
template <std::floating_point T>
class peak_envelope_follower
{
public:
    peak_envelope_follower(std::size_t window_size)
        : m_buffer(window_size)
    {
    }

    auto operator()(T const in) noexcept -> T
    {
        ++m_write_pos;
        if (m_write_pos == m_buffer.size())
        {
            m_write_pos = 0;
        }

        m_buffer[m_write_pos] = std::abs(in);

        if (m_write_pos == m_peak_pos)
        {
            m_peak_pos = std::distance(
                    m_buffer.begin(),
                    std::ranges::max_element(m_buffer));
        }
        else if (m_buffer[m_write_pos] > m_buffer[m_peak_pos])
        {
            m_peak_pos = m_write_pos;
        }

        return m_buffer[m_peak_pos];
    }

private:
    std::size_t m_write_pos{};
    std::size_t m_peak_pos{};
    std::vector<T> m_buffer{};
};

} // namespace piejam::audio::dsp
