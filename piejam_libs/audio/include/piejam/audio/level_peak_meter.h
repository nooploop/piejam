// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cmath>
#include <cstddef>

namespace piejam::audio
{

class level_peak_meter
{
public:
    using value_type = float;

    level_peak_meter(std::size_t decay_time)
        : m_decay_factor(1.f - 1.f / decay_time)
    {
    }

    void push_back(float const x)
    {
        auto const abs_x = std::abs(x);
        auto const decayed = m_peak_level * m_decay_factor;
        m_peak_level = abs_x > decayed ? abs_x : decayed;
    }

    auto get() const noexcept -> float { return m_peak_level; }

    void clear() noexcept { m_peak_level = 0.f; }

private:
    float m_peak_level{};
    float m_decay_factor{.5f};
};

} // namespace piejam::audio
