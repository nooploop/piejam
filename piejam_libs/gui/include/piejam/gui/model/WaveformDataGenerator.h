// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/WaveformData.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/math.h>

namespace piejam::gui::model
{

class WaveformDataGenerator
{
public:
    explicit WaveformDataGenerator(int samplesPerPixel = 1)
        : m_samplesPerPixel{samplesPerPixel}
    {
    }

    template <class Samples>
    auto process(Samples const& samples) -> WaveformData
    {
        WaveformData result;

        constexpr auto clip = [](float x) { return math::clamp(x, -1.f, 1.f); };

        for (auto const sample : samples)
        {
            if (m_accNumSamples) [[likely]]
            {
                m_accY0 = std::min(m_accY0, sample);
                m_accY1 = std::max(m_accY1, sample);
            }
            else
            {
                m_accY0 = sample;
                m_accY1 = sample;
            }

            ++m_accNumSamples;

            if (m_accNumSamples >= m_samplesPerPixel)
            {
                result.push_back(clip(m_accY0), clip(m_accY1));

                m_accNumSamples = 0;
            }
        }

        return result;
    }

    void clear()
    {
        m_accY0 = 0;
        m_accY1 = 0;
        m_accNumSamples = 0;
    }

private:
    int m_samplesPerPixel;

    float m_accY0{};
    float m_accY1{};
    int m_accNumSamples{};
};

} // namespace piejam::gui::model
