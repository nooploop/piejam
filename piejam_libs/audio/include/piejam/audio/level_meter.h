// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sample_rate.h>

#include <cmath>

namespace piejam::audio
{

class level_meter
{
public:
    using value_type = float;

    explicit level_meter(sample_rate sr)
        : m_g_release{std::pow(9.f, (-1.f / (0.4f * sr.as_float())))}
    {
    }

    void push_back(float const x)
    {
        auto const abs_x = std::abs(x);
        m_peak_level = abs_x > m_peak_level
                               ? abs_x
                               : abs_x + m_g_release * (m_peak_level - abs_x);
    }

    [[nodiscard]] auto peak_level() const noexcept -> float
    {
        return m_peak_level;
    }

private:
    float m_peak_level{};
    float m_g_release{};
};

} // namespace piejam::audio
