// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopeLinesAccumulator.h>

#include <piejam/gui/model/ScopeLines.h>

#include <boost/assert.hpp>

#include <stdexcept>

namespace piejam::gui::model
{

void
ScopeLinesAccumulator::setSamplesPerLine(int x)
{
    if (x <= 0)
        throw std::invalid_argument("samplesPerPoint must be positive");

    if (m_samplesPerPoint != x)
    {
        m_samplesPerPoint = x;
        emit samplesPerLineChanged();
    }
}

void
ScopeLinesAccumulator::update(std::span<float const> const& samples)
{
    BOOST_ASSERT(samples.size() % 2 == 0);
    if (samples.empty())
        return;

    ScopeLines resultLeft, resultRight;
    std::size_t const resultCapacity = samples.size() / m_samplesPerPoint + 1;
    resultLeft.reserve(resultCapacity);
    resultRight.reserve(resultCapacity);

    for (std::size_t i = 0, e = samples.size(); i < e; i += 2)
    {
        float const leftSample = samples[i];
        float const rightSample = samples[i + 1];

        if (m_accNumSamples) [[likely]]
        {
            m_accLeftY0 = std::min(m_accLeftY0, leftSample);
            m_accLeftY1 = std::max(m_accLeftY1, leftSample);
            m_accRightY0 = std::min(m_accRightY0, rightSample);
            m_accRightY1 = std::max(m_accRightY1, rightSample);
        }
        else
        {
            m_accLeftY0 = m_accLeftY1 = leftSample;
            m_accRightY0 = m_accRightY1 = rightSample;
        }

        ++m_accNumSamples;

        if (m_accNumSamples >= m_samplesPerPoint)
        {
            constexpr auto clip = [](float x) {
                return std::clamp(x, -1.f, 1.f);
            };

            resultLeft.push_back(clip(m_accLeftY0), clip(m_accLeftY1));
            resultRight.push_back(clip(m_accRightY0), clip(m_accRightY1));

            m_accNumSamples = 0;
        }
    }

    BOOST_ASSERT(resultLeft.size() == resultRight.size());
    if (!resultLeft.empty())
        emit linesAdded(resultLeft, resultRight);
}

} // namespace piejam::gui::model
